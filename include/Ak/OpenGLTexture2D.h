#pragma once

#include <glad/glad.h>

#include <glm/fwd.hpp>

namespace Ak {

class OpenGLTexture2D final
{
public:
  OpenGLTexture2D();

  OpenGLTexture2D(OpenGLTexture2D&& other);

  ~OpenGLTexture2D();

  OpenGLTexture2D(const OpenGLTexture2D&) = delete;

  /// Opens an image file and loads the data onto the texture.
  ///
  /// @note The texture must be bound before calling this function.
  ///
  /// @param path The path of the image to open.
  ///
  /// @param flipVertically Whether or not the image should be flipped when loaded.
  ///
  /// @return True on success, false on failure.
  bool openFile(const char* path, bool flipVertically = true);

  /// Saves the contents of the texture to a PNG file.
  ///
  /// @note The texture must be bound before calling this function.
  ///
  /// @param path The path to save the file to.
  ///
  /// @param format The format to read the texture in. Only 1, 3, and 4 channel formats are supported.
  ///
  /// @param flipVertically Whether or not the image should be flipped before saving.
  ///
  /// @return True on success, false on failure.
  bool savePNG(const char* path, GLenum format = GL_RGBA, bool flipVertically = true);

  GLuint id() noexcept { return m_textureID; }

  bool isBound() const noexcept { return m_boundFlag; }

  void bind();

  void unbind();

  void resize(GLint w, GLint h, GLenum format, GLenum type);

  void write(GLint x, GLint y, GLint w, GLint h, GLenum format, GLenum type, const void* pixels);

  void write(GLint x, GLint y, GLint w, GLint h, const float* r);

  void write(GLint x, GLint y, GLint w, GLint h, const glm::vec3* rgb);

  void write(GLint x, GLint y, GLint w, GLint h, const glm::vec4* rgba);

  void read(GLint level, GLenum format, GLenum type, void* pixels) const;

  void read(GLint level, float* rgb) const;

  void read(GLint level, glm::vec3* rgb) const;

  void read(GLint level, glm::vec4* rgb) const;

  bool isCreated() const noexcept { return m_textureID > 0; }

private:
  GLuint m_textureID = 0;

  bool m_boundFlag = false;
};

} // namespace Ak
