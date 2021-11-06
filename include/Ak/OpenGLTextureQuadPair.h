#pragma once

#include <glad/glad.h>

#include <Ak/OpenGLShaderProgram.h>

namespace Ak {

class OpenGLTexture2D;

class OpenGLTextureQuadPair final
{
public:
  class RenderProgram final : public OpenGLShaderProgramTemplate<RenderProgram>
  {
  public:
    RenderProgram();

    void render(const OpenGLTextureQuadPair&);
  };

  OpenGLTextureQuadPair(const OpenGLTexture2D& texturRef);

  OpenGLTextureQuadPair(OpenGLTextureQuadPair&) = delete;

  ~OpenGLTextureQuadPair();

  const OpenGLTexture2D& getTextureRef() const;

  GLuint getVertexArrayObjectID() const { return m_vertexArrayObject; }

  GLuint getVertexBufferID() const { return m_vertexBuffer; }

private:
  GLuint m_vertexArrayObject = 0;

  GLuint m_vertexBuffer = 0;

  const OpenGLTexture2D& m_texture;
};

} // namespace Ak
