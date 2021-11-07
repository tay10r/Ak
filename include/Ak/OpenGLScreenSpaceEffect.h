#pragma once

#include <Ak/OpenGLShaderProgram.h>

#include <Ak/OpenGLVertexBuffer.h>

namespace Ak {

template<typename Derived>
class OpenGLScreenSpaceEffect : public OpenGLShaderProgramTemplate<Derived>
{
public:
  OpenGLScreenSpaceEffect(const char* vertSource, const char* fragSource);

  virtual ~OpenGLScreenSpaceEffect() = default;

protected:
  void bindQuad() { m_quad.bind(); }

  void unbindQuad() { m_quad.unbind(); }

private:
  OpenGLVertexBuffer<glm::vec2> m_quad;
};

template<typename Derived>
OpenGLScreenSpaceEffect<Derived>::OpenGLScreenSpaceEffect(const char* vertSource, const char* fragSource)
  : OpenGLShaderProgramTemplate<Derived>(vertSource, fragSource)
{
  // clang-format off
  OpenGLVertexBuffer<glm::vec2>::Vertex vertices[6]{
    { glm::vec2(0, 0) }, { glm::vec2(0, 1) }, { glm::vec2(1, 0) },
    { glm::vec2(1, 0) }, { glm::vec2(0, 1) }, { glm::vec2(1, 1) }
  };
  // clang-format on

  m_quad.bind();

  m_quad.allocate(6, GL_STATIC_DRAW);

  m_quad.write(0, vertices, 6);

  m_quad.unbind();
}

} // namespace Ak
