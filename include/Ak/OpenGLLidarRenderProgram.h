#pragma once

#include <Ak/OpenGLShaderProgram.h>

#include <glm/fwd.hpp>

namespace Ak {

template<typename... Attribs>
class OpenGLVertexBuffer;

class OpenGLLidarRenderProgram final
{
public:
  OpenGLLidarRenderProgram();

  void setMVP(const glm::mat4& mvp);

  void render(const OpenGLVertexBuffer<glm::vec3, float>& buffer);

private:
  OpenGLShaderProgram m_renderLidarProgram;

  OpenGLShaderProgram m_renderLidarLightingProgram;

  /// The MVP location for the point rendering program.
  GLint m_mvpLocation = -1;
};

} // namespace Ak
