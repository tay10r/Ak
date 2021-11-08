#include <Ak/FlyCamera.h>
#include <Ak/GLFW.h>
#include <Ak/OpenGLLidarRenderProgram.h>
#include <Ak/OpenGLVertexBuffer.h>
#include <Ak/SingleWindowGLFWApp.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <random>
#include <vector>

#include <cstdio>
#include <cstdlib>

namespace {

class App final : public Ak::SingleWindowGLFWApp
{
public:
  App(Ak::OpenGLVertexBuffer<glm::vec3, float>&& lidarPoints, Ak::GLFWWindow& window)
    : m_lidarPoints(std::move(lidarPoints))
  {
    glClearColor(0, 0, 0, 1);

    window.registerEventObserver(m_camera.makeGLFWEventProxy());
  }

  const char* title() const noexcept override { return "LiDAR Renderer"; }

  void requestAnimationFrame(Ak::GLFWWindow&) override
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = m_camera.worldToCameraMatrix();

    glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);

    glm::mat4 mvp = proj * view;

    m_lidarRenderProgram.setEye(m_camera.position());

    m_lidarRenderProgram.setMVP(mvp);

    m_lidarPoints.bind();

    m_lidarRenderProgram.render(m_lidarPoints);

    m_lidarPoints.unbind();
  }

private:
  Ak::OpenGLLidarRenderProgram m_lidarRenderProgram;

  Ak::OpenGLVertexBuffer<glm::vec3, float> m_lidarPoints;

  Ak::FlyCamera<float> m_camera;
};

static Ak::SingleWindowGLFWApp*
makeApp(int argc, char** argv, Ak::GLFWWindow& window)
{
  if (argc != 2) {
    std::fprintf(stderr, "usage: %s <lidar-points.txt>\n", argv[0]);
    return nullptr;
  }

  const char* pointsFilePath = argv[1];

  std::ifstream pointsFile(pointsFilePath);

  if (!pointsFile.good()) {
    std::fprintf(stderr, "%s: failed to open '%s'\n", argv[0], pointsFilePath);
    return nullptr;
  }

  std::vector<Ak::OpenGLVertexBuffer<glm::vec3, float>::Vertex> points;

  while (pointsFile) {

    Ak::OpenGLVertexBuffer<glm::vec3, float>::Vertex vertex;

    float x = 0;
    float y = 0;
    float z = 0;

    if (!(pointsFile >> x) || !(pointsFile >> y) || !(pointsFile >> z))
      break;

    glm::vec3& position = vertex.attribAt<0>();
    position.x = y;
    position.y = z;
    position.z = -x;

    float& intensity = vertex.attribAt<1>();

    if (!(pointsFile >> intensity))
      break;

    points.emplace_back(vertex);
  }

  Ak::OpenGLVertexBuffer<glm::vec3, float> lidarPoints;

  lidarPoints.bind();

  lidarPoints.allocate(points.size(), GL_STATIC_DRAW);

  lidarPoints.write(0, &points[0], points.size());

  lidarPoints.unbind();

  return new App(std::move(lidarPoints), window);
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
