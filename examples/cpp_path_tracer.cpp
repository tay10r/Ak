#include <Ak/FlyCamera.h>
#include <Ak/GLFW.h>
#include <Ak/ObjMeshModel.h>
#include <Ak/OpenGLHRTMeshRenderProgram.h>
#include <Ak/OpenGLTexture2D.h>
#include <Ak/OpenGLTextureQuadPair.h>
#include <Ak/OpenGLVertexBuffer.h>
#include <Ak/RTMeshModel.h>
#include <Ak/SingleWindowGLFWApp.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <vector>

#include <cassert>
#include <cstdio>
#include <cstdlib>

namespace {

struct Framebuffer final
{
  Ak::OpenGLTexture2D colorTexture;

  std::vector<glm::vec3> rgb;

  std::vector<std::minstd_rand> rng;

  int width = 0;

  int height = 0;

  int size() const noexcept { return width * height; }
};

class FramebufferResizer final : public Ak::GLFWEventObserver
{
public:
  FramebufferResizer(const std::shared_ptr<Framebuffer>& framebuffer)
    : m_framebuffer(framebuffer)
  {}

  void resizeEvent(int w, int h) override
  {
    m_framebuffer->width = w;

    m_framebuffer->height = h;

    m_framebuffer->rgb.resize(w * h);

    m_framebuffer->rng.resize(w * h);

    std::seed_seq seed{ w, h, 1234 };

    std::mt19937 seedRng(seed);

    for (int i = 0; i < m_framebuffer->rng.size(); i++)
      m_framebuffer->rng[i] = std::minstd_rand(seedRng());

    m_framebuffer->colorTexture.bind();

    m_framebuffer->colorTexture.resize(w, h, GL_RGB32F, GL_RGB, GL_FLOAT);

    m_framebuffer->colorTexture.unbind();
  }

private:
  std::shared_ptr<Framebuffer> m_framebuffer;
};

struct OpenGLShape final
{
  Ak::OpenGLVertexBuffer<glm::vec3, glm::vec3, glm::vec2> vertexBuffer;
};

class CPPIndirectLightingPass final
{
public:
  CPPIndirectLightingPass(const Ak::RTMeshModel<float>& rtMeshModel)
    : m_rtMeshModel(rtMeshModel)
  {
    const int w = 640;
    const int h = 480;

    m_noiseTexture.resize(w * h);

    std::seed_seq seed{ 1234, w, h };

    std::mt19937 rng(seed);

    std::uniform_real_distribution<float> noiseTexelDist(0, 1);

    for (glm::vec3& noiseTexel : m_noiseTexture)
      noiseTexel = glm::vec3(noiseTexelDist(rng), noiseTexelDist(rng), noiseTexelDist(rng));
  }

  static glm::vec3 normalizeByW(const glm::vec4& v) { return glm::vec3(v.x / v.w, v.y / v.w, v.z / v.w); }

  void execute(const std::vector<glm::vec4>& normalDepthTex,
               const glm::mat4& inverseMVP,
               int w,
               int h,
               std::vector<glm::vec3>& outTex) const
  {
#pragma omp parallel for
    for (int y = 0; y < h; y++) {

      for (int x = 0; x < w; x++) {

        const float u = float(x) / w;
        const float v = float(y) / h;

        const int texelIndex = (y * w) + x;

        const glm::vec4 normalDepth = normalDepthTex[texelIndex];

        const float xNDC = (u * 2) - 1;
        const float yNDC = (v * 2) - 1;
        const float zNDC = normalDepth.w;

        const glm::vec4 worldSpacePoint = inverseMVP * glm::vec4(xNDC, yNDC, zNDC, 1.0f);

        const glm::vec3 normal(normalDepth.x, normalDepth.y, normalDepth.z);

        if (normalDepth.w == 1.0f)
          outTex[texelIndex] = glm::vec3(0, 0, 0);
        else
          outTex[texelIndex] = computeTexel(normalizeByW(worldSpacePoint), (normal * 2.0f) * 0.5f, x, y);
      }
    }
  }

private:
  static constexpr int wrap(int x, int min, int max)
  {
    if (x < min)
      return max - x;
    else if (x > max)
      return min + (x - max);
    else
      return x;
  }

  glm::vec3 computeTexel(const glm::vec3& pos, const glm::vec3& norm, int xCenter, int yCenter) const
  {
    std::seed_seq seed{ 1234, xCenter, yCenter };

    std::minstd_rand rng(seed);

    const int radius = 1;

    const int xMin = xCenter - radius;
    const int yMin = yCenter - radius;

    const int xMax = xCenter + radius;
    const int yMax = yCenter + radius;

    glm::vec3 sampleSum(0, 0, 0);

    for (int y = yMin; y <= yMax; y++) {

      for (int x = xMin; x <= xMax; x++) {

        const int xTexel = wrap(x, 0, 640);
        const int yTexel = wrap(y, 0, 480);

        sampleSum += computeSample(pos, sampleHemisphere(norm, rng));
      }
    }

    const int sampleCount = ((xMax - xMin) + 1) * ((yMax - yMin) + 1);

    const glm::vec3 out = sampleSum * (1.0f / sampleCount);

    return glm::clamp(out, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
  }

  glm::vec3 computeSample(const glm::vec3& pos, const glm::vec3& rayDir) const
  {
    using Ray = bvh::Ray<float>;

    using Vec3 = bvh::Vector3<float>;

    const float shadowBias = 0.00001;

    const glm::vec3 rayOrg = pos; // + (rayDir * shadowBias);

    const Ray ray(Vec3(rayOrg.x, rayOrg.y, rayOrg.z), Vec3(rayDir.x, rayDir.y, rayDir.z), shadowBias, 100.0f);

    if (m_rtMeshModel.findAnyHit(ray))
      return glm::vec3(0, 0, 0);
    else
      return glm::vec3(1, 1, 1);
  }

  template<typename Rng>
  static glm::vec3 sampleHemisphere(const glm::vec3& normal, Rng& rng)
  {
    std::uniform_real_distribution<float> dist(-1, 1);

    for (int i = 0; i < 128; i++) {

      const glm::vec3 v(dist(rng), dist(rng), dist(rng));

      if ((glm::dot(v, v) <= 1) && (glm::dot(v, normal) >= 0))
        return glm::normalize(v);
    }

    return normal;
  }

private:
  std::vector<glm::vec3> m_noiseTexture;

  const Ak::RTMeshModel<float>& m_rtMeshModel;
};

class App final : public Ak::SingleWindowGLFWApp
{
public:
  static constexpr int fbWidth() { return 640; }

  static constexpr int fbHeight() { return 480; }

  App(Ak::ObjMeshModel&& objMeshModel, Ak::GLFWWindow& window)
    : m_objMeshModel(std::move(objMeshModel))
  {
    m_rtMeshModel.useObjModel(m_objMeshModel);

    m_rtMeshModel.commit();

    m_camera.applyRelativeMove(glm::vec3(0, 1, 5));

    window.registerEventObserver(m_camera.makeGLFWEventProxy());

    std::vector<Ak::ObjMeshModel::ShapeView> shapeViews = m_objMeshModel.getShapeViews();

    for (const Ak::ObjMeshModel::ShapeView& shapeView : shapeViews) {

      OpenGLShape openGLShape;

      openGLShape.vertexBuffer.bind();

      openGLShape.vertexBuffer.allocate(shapeView.vertexCount, GL_STATIC_DRAW);

      using OpenGLVertex = Ak::OpenGLVertexBuffer<glm::vec3, glm::vec3, glm::vec2>::Vertex;

      static_assert(sizeof(OpenGLVertex) == (sizeof(float) * 8));

      openGLShape.vertexBuffer.write(0, (OpenGLVertex*)shapeView.vertexBuffer, shapeView.vertexCount);

      openGLShape.vertexBuffer.unbind();

      m_openGLShapes.emplace_back(std::move(openGLShape));
    }

    std::shared_ptr<Ak::GLFWEventObserver> framebufferResizer(new FramebufferResizer(m_framebuffer));

    window.registerEventObserver(framebufferResizer);

    window.fakeFramebufferResizeEvent();

    glClearColor(0, 0, 0, 1);

    m_hrtMeshRenderProgram.resizeFramebuffer(fbWidth(), fbHeight());
  }

  const char* title() const noexcept override { return "C++ Path Tracer"; }

  void requestAnimationFrame(Ak::GLFWWindow& window) override
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 proj = glm::perspective(glm::radians(45.0f), window.aspectRatio(), 0.1f, 100.0f);

    const glm::mat4 view = m_camera.worldToCameraMatrix();

    const glm::mat4 mvp = proj * view;

    m_hrtMeshRenderProgram.bind();

    assert(m_hrtMeshRenderProgram.isInitialized());

    m_hrtMeshRenderProgram.setMVP(mvp);

    for (OpenGLShape& shape : m_openGLShapes) {

      shape.vertexBuffer.bind();

      m_hrtMeshRenderProgram.render(shape.vertexBuffer);

      shape.vertexBuffer.unbind();
    }

    m_hrtMeshRenderProgram.unbind();

    Ak::OpenGLTexture2D* normalDepthTexture = m_hrtMeshRenderProgram.normalDepthTexture();

    std::vector<glm::vec4> normalDepthData(fbWidth() * fbHeight());
    normalDepthTexture->bind();
    normalDepthTexture->read(0, &normalDepthData[0]);
    normalDepthTexture->unbind();

    CPPIndirectLightingPass indirectLightingPass(m_rtMeshModel);

    std::vector<glm::vec3> out(fbWidth() * fbHeight());

    indirectLightingPass.execute(normalDepthData, glm::inverse(mvp), fbWidth(), fbHeight(), out);

    m_framebuffer->colorTexture.bind();

    m_framebuffer->colorTexture.write(0, 0, fbWidth(), fbHeight(), out.data());

    m_framebuffer->colorTexture.unbind();

    m_textureQuadProgram.render(m_textureQuad);
  }

private:
  Ak::FlyCamera<float> m_camera;

  Ak::ObjMeshModel m_objMeshModel;

  Ak::RTMeshModel<float> m_rtMeshModel;

  Ak::OpenGLHRTMeshRenderProgram m_hrtMeshRenderProgram;

  std::vector<OpenGLShape> m_openGLShapes;

  std::shared_ptr<Framebuffer> m_framebuffer{ new Framebuffer() };

  Ak::OpenGLTextureQuadPair m_textureQuad{ &m_framebuffer->colorTexture };

  Ak::OpenGLTextureQuadPair::RenderProgram m_textureQuadProgram;
};

static Ak::SingleWindowGLFWApp*
makeApp(int argc, char** argv, Ak::GLFWWindow& window)
{
  if (argc != 2) {
    std::fprintf(stderr, "usage: %s <model.obj>\n", argv[0]);
    return nullptr;
  }

  const char* objPath = argv[1];

  Ak::ObjMeshModel objMeshModel;

  if (!objMeshModel.loadFile(objPath)) {
    std::fprintf(stderr, "%s: failed to load '%s'\n", argv[0], objPath);
    return nullptr;
  }

  return new App(std::move(objMeshModel), window);
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
