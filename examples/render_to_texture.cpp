#include <Ak/FlyCamera.h>
#include <Ak/GLFW.h>
#include <Ak/OpenGLFramebuffer.h>
#include <Ak/OpenGLPointRenderProgram.h>
#include <Ak/OpenGLRenderbuffer.h>
#include <Ak/OpenGLTexture2D.h>
#include <Ak/OpenGLVertexBuffer.h>
#include <Ak/SingleWindowGLFWApp.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <random>
#include <vector>

#include <cstdio>
#include <cstdlib>

namespace {

class App final : public Ak::SingleWindowGLFWApp
{
public:
  App()
  {
    const std::vector<Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4>::Vertex> points = generateRandomPoints(1'000'000);

    m_pointBuffer.bind();

    m_pointBuffer.allocate(points.size(), GL_STATIC_DRAW);

    m_pointBuffer.write(0, &points[0], points.size());

    m_pointBuffer.unbind();

    // Setup color attachment

    m_colorAttachment.bind();

    m_colorAttachment.resize(m_renderWidth, m_renderHeight, GL_RGBA32F, GL_RGBA, GL_FLOAT);

    m_colorAttachment.unbind();

    // Setup depth attachment

    m_depthAttachment.bind();

    m_depthAttachment.resize(m_renderWidth, m_renderHeight);

    m_depthAttachment.unbind();

    // Setup framebuffer

    m_framebuffer.bind();

    m_depthAttachment.bind();

    m_colorAttachment.bind();

    m_framebuffer.attach(m_depthAttachment);

    m_framebuffer.attach(m_colorAttachment);

    m_colorAttachment.unbind();

    m_depthAttachment.unbind();

    assert(m_framebuffer.isComplete());

    glViewport(0, 0, m_renderWidth, m_renderHeight);

    render();

    m_framebuffer.unbind();

    m_colorAttachment.bind();

    m_colorAttachment.savePNG("result.png");

    m_colorAttachment.unbind();
  }

  const char* title() const noexcept override { return "Point Renderer"; }

  void render()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pointRenderProgram.bind();

    m_pointBuffer.bind();

    glm::mat4 view = glm::lookAt(glm::vec3(4, 4, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glm::mat4 mvp = proj * view;

    m_pointRenderProgram.setMVP(mvp);

    m_pointRenderProgram.render(m_pointBuffer);

    m_pointBuffer.unbind();

    m_pointRenderProgram.unbind();
  }

  void requestAnimationFrame(Ak::GLFWWindow&) override {}

private:
  std::vector<Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4>::Vertex> generateRandomPoints(size_t count)
  {
    std::seed_seq seed{ count, size_t(1234) };

    std::mt19937 rng(seed);

    std::uniform_real_distribution<float> colorDist(0, 1);

    std::uniform_real_distribution<float> posDist(-100, 100);

    std::vector<Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4>::Vertex> vertices(count);

    for (size_t i = 0; i < count; i++) {

      vertices[i].attribAt<0>() = glm::vec3(posDist(rng), posDist(rng), posDist(rng));

      vertices[i].attribAt<1>() = glm::vec4(colorDist(rng), colorDist(rng), colorDist(rng), 1.0f);
    }

    return vertices;
  }

private:
  int m_renderWidth = 640;

  int m_renderHeight = 480;

  Ak::OpenGLRenderbuffer m_depthAttachment;

  Ak::OpenGLTexture2D m_colorAttachment;

  Ak::OpenGLFramebuffer m_framebuffer;

  Ak::OpenGLPointRenderProgram m_pointRenderProgram;

  Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4> m_pointBuffer;
};

static Ak::SingleWindowGLFWApp*
makeApp(int, char**, Ak::GLFWWindow&)
{
  return new App();
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
