#pragma once

#include <glad/glad.h>

#include <vector>

namespace Ak {

class OpenGLRenderbuffer;
class OpenGLTexture2D;

class OpenGLFramebuffer
{
public:
  OpenGLFramebuffer();

  OpenGLFramebuffer(const OpenGLFramebuffer&) = delete;

  ~OpenGLFramebuffer();

  bool isBound() const noexcept { return m_boundFlag; }

  void bind();

  void unbind();

  void attach(OpenGLRenderbuffer&);

  void attach(OpenGLTexture2D& colorAttachment);

  void attach(const std::vector<OpenGLTexture2D*>& colorAttachments);

private:
  GLuint m_framebufferID = 0;

  bool m_boundFlag = false;
};

} // namespace Ak
