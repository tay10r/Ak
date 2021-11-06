#include <Ak/FlyCamera.h>
#include <Ak/GLFW.h>
#include <Ak/OpenGLPointRenderProgram.h>
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
  App(Ak::GLFWWindow& window)
  {
    glClearColor(0, 0, 0, 1);

    window.registerEventObserver(m_camera.makeGLFWEventProxy());

    const std::vector<Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4>::Vertex> points = generateRandomPoints(1'000'000);

    m_pointBuffer.bind();

    m_pointBuffer.allocate(points.size(), GL_STATIC_DRAW);

    m_pointBuffer.write(0, &points[0], points.size());

    m_pointBuffer.unbind();
  }

  const char* title() const noexcept override { return "Point Renderer"; }

  void requestAnimationFrame(Ak::GLFWWindow&) override
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pointRenderProgram.bind();

    m_pointBuffer.bind();

    glm::mat4 view = m_camera.worldToCameraMatrix();

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glm::mat4 mvp = proj * view;

    m_pointRenderProgram.setMVP(mvp);

    m_pointRenderProgram.render(m_pointBuffer);

    m_pointBuffer.unbind();

    m_pointRenderProgram.unbind();
  }

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
  Ak::OpenGLPointRenderProgram m_pointRenderProgram;

  Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4> m_pointBuffer;

  Ak::FlyCamera<float> m_camera;
};

static Ak::SingleWindowGLFWApp*
makeApp(int, char**, Ak::GLFWWindow& window)
{
  return new App(window);
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
