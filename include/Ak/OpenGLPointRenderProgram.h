#pragma once

#include <Ak/OpenGLShaderProgram.h>

#include <glm/fwd.hpp>

namespace Ak {

template<typename...>
class OpenGLVertexBuffer;

class OpenGLPointRenderProgram final : public OpenGLShaderProgramTemplate<OpenGLPointRenderProgram>
{
public:
  OpenGLPointRenderProgram();

  void setMVP(const glm::mat4& mvp);

  void render(OpenGLVertexBuffer<glm::vec3, glm::vec4>& vertexBuffer);

private:
  GLint m_mvpLocation = -1;
};

} // namespace Ak
