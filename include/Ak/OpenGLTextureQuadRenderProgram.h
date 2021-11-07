#pragma once

#include <glad/glad.h>

#include <Ak/OpenGLShaderProgram.h>

#include <glm/fwd.hpp>

namespace Ak {

class OpenGLTexture2D;

template<typename... Attributes>
class OpenGLVertexBuffer;

class OpenGLTextureQuadRenderProgram final : public OpenGLShaderProgramTemplate<OpenGLTextureQuadRenderProgram>
{
public:
  OpenGLTextureQuadRenderProgram();

  void setMVP(const glm::mat4& mvp);

  void render(OpenGLVertexBuffer<glm::vec2>& vertexBuffer, OpenGLTexture2D& texture);

private:
  GLint m_mvpLocation = -1;
};

} // namespace Ak
