#include <Ak/OpenGLLidarRenderProgram.h>

#include <Ak/OpenGLVertexBuffer.h>

#include <cassert>

namespace Ak {

OpenGLLidarRenderProgram::OpenGLLidarRenderProgram()
  : m_renderLidarProgram("shaders/render_lidar.vert", "shaders/render_lidar.frag")
  , m_renderLidarLightingProgram("shaders/render_lidar_lighting.vert", "shaders/render_lidar_lighting.frag")
{
  m_renderLidarProgram.bind();

  m_mvpLocation = m_renderLidarProgram.getUniformLocation("mvp");

  m_renderLidarProgram.unbind();

  assert(m_mvpLocation >= 0);
}

void
OpenGLLidarRenderProgram::setMVP(const glm::mat4& mvp)
{
  m_renderLidarProgram.bind();

  m_renderLidarProgram.setUniformValue(m_mvpLocation, mvp);

  m_renderLidarProgram.unbind();
}

void
OpenGLLidarRenderProgram::render(const OpenGLVertexBuffer<glm::vec3, float>& lidarPoints)
{
  assert(lidarPoints.isBound());

  m_renderLidarProgram.bind();

  glDrawArrays(GL_POINTS, 0, lidarPoints.getVertexCount());

  m_renderLidarProgram.unbind();
}

} // namespace Ak
