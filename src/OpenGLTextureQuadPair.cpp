#include <Ak/OpenGLTextureQuadPair.h>

#include <Ak/OpenGLTexture2D.h>

namespace Ak {

OpenGLTextureQuadPair::RenderProgram::RenderProgram()
  : OpenGLShaderProgramTemplate<RenderProgram>("shaders/render_texture_quad_pair.vert",
                                               "shaders/render_texture_quad_pair.frag")
{}

OpenGLTextureQuadPair::OpenGLTextureQuadPair(const OpenGLTexture2D& texture)
  : m_texture(texture)
{
  glGenVertexArrays(1, &m_vertexArrayObject);

  glBindVertexArray(m_vertexArrayObject);

  glGenBuffers(1, &m_vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

  // clang-format off
  const float vertices[12] {
    0, 0,
    0, 1,
    1, 0,
    1, 0,
    0, 1,
    1, 1
  };
  // clang-format on

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glBindVertexArray(0);
}

OpenGLTextureQuadPair::~OpenGLTextureQuadPair()
{
  if (m_vertexArrayObject) {

    glBindVertexArray(m_vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);

    glBindVertexArray(0);
  }

  if (m_vertexBuffer)
    glDeleteBuffers(1, &m_vertexBuffer);

  if (m_vertexArrayObject)
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

const OpenGLTexture2D&
OpenGLTextureQuadPair::getTextureRef() const
{
  return m_texture;
}

void
OpenGLTextureQuadPair::RenderProgram::render(const OpenGLTextureQuadPair& textureQuadPair)
{
  bind();

  glBindVertexArray(textureQuadPair.getVertexArrayObjectID());

  textureQuadPair.getTextureRef().bind();

  glDrawArrays(GL_TRIANGLES, 0, 6);

  textureQuadPair.getTextureRef().unbind();

  glBindVertexArray(0);

  unbind();
}

} // namespace Ak
