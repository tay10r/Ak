#pragma once

#include <Ak/OpenGLShaderProgram.h>

#include <Ak/OpenGLVertexBuffer.h>

namespace Ak {

class OpenGLScreenSpaceEffect : public OpenGLShaderProgramTemplate<OpenGLScreenSpaceEffect>
{
public:
  OpenGLScreenSpaceEffect(const char* vertSource, const char* fragSource);

  virtual ~OpenGLScreenSpaceEffect() = default;

  void bindQuad() { m_quad.bind(); }

  void unbindQuad() { m_quad.unbind(); }

private:
  OpenGLVertexBuffer<glm::vec2> m_quad;
};

} // namespace Ak
