#include <Ak/GLFW.h>
#include <Ak/OpenGLPointRenderProgram.h>
#include <Ak/OpenGLVertexBuffer.h>
#include <Ak/SingleWindowGLFWApp.h>

#include <glm/glm.hpp>

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
    glClearColor(0, 0, 0, 1);

    Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4>::Vertex vertex;

    vertex.at<0>() = glm::vec3(0, 0, 0);
    vertex.at<1>() = glm::vec4(1, 1, 1, 1);

    m_pointBuffer.bind();

    m_pointBuffer.allocate(1, GL_STATIC_DRAW);

    m_pointBuffer.write(0, &vertex, 1);

    m_pointBuffer.unbind();
  }

  const char* title() const noexcept override { return "Point Renderer"; }

  void requestAnimationFrame(Ak::GLFWWindow&) override
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_pointRenderProgram.bind();

    m_pointBuffer.bind();

    m_pointRenderProgram.render(m_pointBuffer);

    m_pointBuffer.unbind();

    m_pointRenderProgram.unbind();
  }

private:
  Ak::OpenGLPointRenderProgram m_pointRenderProgram;

  Ak::OpenGLVertexBuffer<glm::vec3, glm::vec4> m_pointBuffer;
};

static Ak::SingleWindowGLFWApp*
makeApp(int, char**, Ak::GLFWWindow& window)
{
  return new App();
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
