#pragma once

#include <glad/glad.h>

#include <glm/fwd.hpp>

namespace Ak {

class OpenGLTexture2D final
{
public:
  OpenGLTexture2D();

  ~OpenGLTexture2D();

  OpenGLTexture2D(const OpenGLTexture2D&) = delete;

  GLuint id() noexcept { return m_textureID; }

  bool isBound() const noexcept { return m_boundFlag; }

  void bind();

  void unbind();

  void resize(GLint w, GLint h, GLenum format, GLenum type);

  void write(GLint x, GLint y, GLint w, GLint h, GLenum format, GLenum type, const void* pixels);

  void write(GLint x, GLint y, GLint w, GLint h, const float* r);

  void write(GLint x, GLint y, GLint w, GLint h, const glm::vec3* rgb);

  void write(GLint x, GLint y, GLint w, GLint h, const glm::vec4* rgba);

  bool isCreated() const noexcept { return m_textureID > 0; }

private:
  GLuint m_textureID = 0;

  bool m_boundFlag = false;
};

} // namespace Ak
