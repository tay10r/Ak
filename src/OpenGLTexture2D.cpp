#include <Ak/OpenGLTexture2D.h>

namespace Ak {

OpenGLTexture2D::OpenGLTexture2D()
{
  glGenTextures(1, &m_textureID);

  bind();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  unbind();
}

OpenGLTexture2D::~OpenGLTexture2D()
{
  if (m_textureID)
    glDeleteTextures(1, &m_textureID);
}

void
OpenGLTexture2D::bind() const
{
  glBindTexture(GL_TEXTURE_2D, m_textureID);
}

void
OpenGLTexture2D::unbind()
{
  glBindTexture(GL_TEXTURE_2D, 0);
}

void
OpenGLTexture2D::uploadRGB(const float* rgb, int w, int h)
{
  bind();

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_FLOAT, rgb);

  unbind();
}

} // namespace Ak
