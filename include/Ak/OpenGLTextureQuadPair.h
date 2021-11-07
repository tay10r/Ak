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

    void render(OpenGLTextureQuadPair&);
  };

  OpenGLTextureQuadPair(OpenGLTexture2D* texturePtr);

  OpenGLTextureQuadPair(OpenGLTextureQuadPair&) = delete;

  ~OpenGLTextureQuadPair();

  OpenGLTexture2D* getTexturePtr() const { return m_texturePtr; }

  GLuint getVertexArrayObjectID() const { return m_vertexArrayObject; }

  GLuint getVertexBufferID() const { return m_vertexBuffer; }

private:
  GLuint m_vertexArrayObject = 0;

  GLuint m_vertexBuffer = 0;

  OpenGLTexture2D* m_texturePtr;
};

} // namespace Ak
