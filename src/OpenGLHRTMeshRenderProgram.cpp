#include <Ak/OpenGLHRTMeshRenderProgram.h>

#include <Ak/OpenGLVertexBuffer.h>

#include <cassert>

namespace Ak {

OpenGLHRTMeshRenderProgram::OpenGLHRTMeshRenderProgram()
  : OpenGLShaderProgram(":/shaders/hrt_render_mesh.vert", ":/shaders/hrt_render_mesh.frag")
{
  bind();

  m_mvpLocation = getUniformLocation("mvp");

  assert(m_mvpLocation >= 0);

  unbind();

  m_framebuffer.bind();

  m_renderbuffer.bind();

  m_framebuffer.attach(m_renderbuffer);

  m_renderbuffer.unbind();

  m_framebuffer.attach({ &m_albedoTexture, &m_normalDepthTexture });

  m_framebuffer.unbind();
}

bool
OpenGLHRTMeshRenderProgram::isInitialized()
{
  m_framebuffer.bind();

  const bool isFramebufferComplete = m_framebuffer.isComplete();

  m_framebuffer.unbind();

  return isFramebufferComplete;
}

void
OpenGLHRTMeshRenderProgram::setMVP(const glm::mat4& mvp)
{
  assert(isBound());

  setUniformValue(m_mvpLocation, mvp);
}

void
OpenGLHRTMeshRenderProgram::render(const OpenGLVertexBuffer<glm::vec3, glm::vec3, glm::vec2>& vertexBuffer)
{
  glEnable(GL_DEPTH_TEST);

  assert(vertexBuffer.isBound());

  assert(isBound());

  m_framebuffer.bind();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.getVertexCount());

  m_framebuffer.unbind();

  glDisable(GL_DEPTH_TEST);
}

void
OpenGLHRTMeshRenderProgram::resizeFramebuffer(int w, int h)
{
  m_renderbuffer.bind();
  m_renderbuffer.resize(w, h);
  m_renderbuffer.unbind();

  m_albedoTexture.bind();
  m_albedoTexture.resize(w, h, GL_RGB, GL_RGB, GL_FLOAT);
  m_albedoTexture.unbind();

  m_normalDepthTexture.bind();
  m_normalDepthTexture.resize(w, h, GL_RGBA32F, GL_RGBA, GL_FLOAT);
  m_normalDepthTexture.unbind();
}

} // namespace Ak
