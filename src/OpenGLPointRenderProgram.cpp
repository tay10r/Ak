#include <Ak/OpenGLPointRenderProgram.h>

#include <Ak/OpenGLVertexBuffer.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cassert>

namespace Ak {

OpenGLPointRenderProgram::OpenGLPointRenderProgram()
  : OpenGLShaderProgramTemplate<OpenGLPointRenderProgram>("shaders/render_points.vert", "shaders/render_points.frag")
{
  bind();

  m_mvpLocation = getUniformLocation("mvp");

  assert(m_mvpLocation >= 0);

  unbind();
}

void
OpenGLPointRenderProgram::setMVP(const glm::mat4& mvp)
{
  setUniformValue(m_mvpLocation, mvp);
}

void
OpenGLPointRenderProgram::render(OpenGLVertexBuffer<glm::vec3, glm::vec4>& buffer)
{
  assert(isBound());

  assert(buffer.isBound());

  glDrawArrays(GL_POINTS, 0, buffer.getVertexCount());
}

} // namespace Ak
