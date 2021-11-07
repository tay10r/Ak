#include <Ak/OpenGLFramebuffer.h>

#include <Ak/OpenGLRenderbuffer.h>
#include <Ak/OpenGLTexture2D.h>

#include <cassert>

namespace Ak {

OpenGLFramebuffer::OpenGLFramebuffer()
{
  glGenFramebuffers(1, &m_framebufferID);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
  glDeleteFramebuffers(1, &m_framebufferID);
}

void
OpenGLFramebuffer::bind()
{
  assert(!m_boundFlag);

  glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);

  m_boundFlag = true;
}

void
OpenGLFramebuffer::unbind()
{
  assert(m_boundFlag);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_boundFlag = false;
}

void
OpenGLFramebuffer::attach(OpenGLRenderbuffer& renderbuffer)
{
  assert(m_boundFlag);

  assert(renderbuffer.isBound());

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer.id());
}

void
OpenGLFramebuffer::attach(OpenGLTexture2D& texture)
{
  return attach({ &texture });
}

void
OpenGLFramebuffer::attach(const std::vector<OpenGLTexture2D*>& textures)
{
  assert(m_boundFlag);

  assert(textures.size() <= 8);

  std::vector<GLenum> drawBuffers;

  for (int i = 0; (i < int(textures.size())) && (i < 8); i++) {

    const GLenum colorAttachment = GL_COLOR_ATTACHMENT0 + i;

    drawBuffers.emplace_back(colorAttachment);

    glFramebufferTexture(GL_FRAMEBUFFER, colorAttachment, textures[i]->id(), 0);
  }

  glDrawBuffers(drawBuffers.size(), &drawBuffers[0]);
}

bool
OpenGLFramebuffer::isComplete() const
{
  assert(m_boundFlag);

  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

} // namespace Ak
