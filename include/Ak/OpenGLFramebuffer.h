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

  /// @note The framebuffer must be bound before calling this function.
  bool isComplete() const;

  void bind();

  void unbind();

  /// @note The framebuffer must be bound before calling this function.
  void attach(OpenGLRenderbuffer&);

  /// @note The framebuffer must be bound before calling this function.
  void attach(OpenGLTexture2D& colorAttachment);

  /// @note The framebuffer must be bound before calling this function.
  void attach(const std::vector<OpenGLTexture2D*>& colorAttachments);

private:
  GLuint m_framebufferID = 0;

  bool m_boundFlag = false;
};

} // namespace Ak
