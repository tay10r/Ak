#pragma once

#include <glad/glad.h>

namespace Ak {

class OpenGLTexture2D final
{
public:
  OpenGLTexture2D();

  ~OpenGLTexture2D();

  OpenGLTexture2D(const OpenGLTexture2D&) = delete;

  void bind() const;

  static void unbind();

  bool isCreated() const noexcept { return m_textureID > 0; }

  void uploadRGB(const unsigned char* rgb, int w, int h);

  void uploadRGB(const float* rgb, int w, int h);

private:
  GLuint m_textureID = 0;
};

} // namespace Ak
