#include <Ak/OpenGLTexture2D.h>

#include <cassert>

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
OpenGLTexture2D::bind()
{
  assert(m_boundFlag == false);

  glBindTexture(GL_TEXTURE_2D, m_textureID);

  m_boundFlag = true;
}

void
OpenGLTexture2D::unbind()
{
  assert(m_boundFlag == true);

  glBindTexture(GL_TEXTURE_2D, 0);

  m_boundFlag = false;
}

void
OpenGLTexture2D::resize(GLint w, GLint h, GLenum format, GLenum type)
{
  assert(m_boundFlag);

  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, type, nullptr);
}

void
OpenGLTexture2D::write(GLint x, GLint y, GLint w, GLint h, GLenum format, GLenum type, const void* pixels)
{
  assert(m_boundFlag);

  glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, format, type, pixels);
}

void
OpenGLTexture2D::write(GLint x, GLint y, GLint w, GLint h, const float* r)
{
  write(x, y, w, h, GL_R, GL_FLOAT, r);
}

void
OpenGLTexture2D::write(GLint x, GLint y, GLint w, GLint h, const glm::vec3* rgb)
{
  write(x, y, w, h, GL_RGB, GL_FLOAT, rgb);
}

void
OpenGLTexture2D::write(GLint x, GLint y, GLint w, GLint h, const glm::vec4* rgba)
{
  write(x, y, w, h, GL_RGBA, GL_FLOAT, rgba);
}

} // namespace Ak
