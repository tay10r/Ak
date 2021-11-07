#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <cassert>
#include <cstddef>

namespace Ak {

template<typename Attrib>
struct OpenGLVertexAttribTraits final
{};

template<>
struct OpenGLVertexAttribTraits<float> final
{
  static constexpr GLint size() { return 1; }

  static constexpr GLenum type() { return GL_FLOAT; }

  static constexpr GLboolean normalized() { return GL_FALSE; }
};

template<>
struct OpenGLVertexAttribTraits<glm::vec2> final
{
  static constexpr GLint size() { return 2; }

  static constexpr GLenum type() { return GL_FLOAT; }

  static constexpr GLboolean normalized() { return GL_FALSE; }
};

template<>
struct OpenGLVertexAttribTraits<glm::vec3> final
{
  static constexpr GLint size() { return 3; }

  static constexpr GLenum type() { return GL_FLOAT; }

  static constexpr GLboolean normalized() { return GL_FALSE; }
};

template<>
struct OpenGLVertexAttribTraits<glm::vec4> final
{
  static constexpr GLint size() { return 4; }

  static constexpr GLenum type() { return GL_FLOAT; }

  static constexpr GLboolean normalized() { return GL_FALSE; }
};

template<typename... Attribs>
class OpenGLVertexBuffer final
{
public:
  template<typename Attrib, typename... Others>
  struct GenericVertex final
  {
    Attrib attrib;

    GenericVertex<Others...> others;

    template<int index>
    constexpr auto& attribAt() noexcept
    {
      if constexpr (index == 0)
        return attrib;
      else
        return others.template attribAt<index - 1>();
    }

    static constexpr size_t bytesPerVertex() { return sizeof(Attrib) + GenericVertex<Others...>::bytesPerVertex(); }

    static void enableAll(GLuint attribIndex, size_t offset, size_t bytesPerVertex)
    {
      using Traits = OpenGLVertexAttribTraits<Attrib>;

      glEnableVertexAttribArray(attribIndex);

      glVertexAttribPointer(
        attribIndex, Traits::size(), Traits::type(), Traits::normalized(), bytesPerVertex, (const void*)offset);

      GenericVertex<Others...>::enableAll(attribIndex + 1, offset + sizeof(Attrib), bytesPerVertex);
    }
  };

  template<typename LastAttrib>
  struct GenericVertex<LastAttrib> final
  {
    LastAttrib attrib;

    template<int index>
    constexpr LastAttrib& attribAt() noexcept
    {
      static_assert(index == 0, "Attribute index is out of bounds.");

      return attrib;
    }

    static constexpr size_t bytesPerVertex() { return sizeof(LastAttrib); }

    static void enableAll(GLuint attribIndex, size_t offset, size_t bytesPerVertex)
    {
      using Traits = OpenGLVertexAttribTraits<LastAttrib>;

      glEnableVertexAttribArray(attribIndex);

      glVertexAttribPointer(
        attribIndex, Traits::size(), Traits::type(), Traits::normalized(), bytesPerVertex, (const void*)offset);
    }
  };

  using Vertex = GenericVertex<Attribs...>;

  OpenGLVertexBuffer();

  OpenGLVertexBuffer(const OpenGLVertexBuffer&) = delete;

  ~OpenGLVertexBuffer();

  void bind();

  void unbind();

  bool isBound() const noexcept { return m_boundFlag; }

  /// Resizes the buffer to fit a given number of vertices.
  ///
  /// @param vertexCount The number of vertices to allocate room for.
  void allocate(size_t vertexCount, GLenum usage);

  /// @note Must be allocated with @ref OpenGLVertexBuffer::resize before calling this function.
  ///
  /// @param offset The index of the vertex to start writing the data to.
  ///
  /// @param data The vertices to write to the buffer.
  ///
  /// @param vertexCount The number of vertices to write to the buffer.
  void write(size_t offset, const Vertex* data, size_t vertexCount);

  size_t vertexCount();

private:
  GLuint m_vertexBuffer = 0;

  GLuint m_vertexArrayObject = 0;

  bool m_boundFlag = false;
};

template<typename... Attribs>
OpenGLVertexBuffer<Attribs...>::OpenGLVertexBuffer()
{
  glGenVertexArrays(1, &m_vertexArrayObject);

  glBindVertexArray(m_vertexArrayObject);

  glGenBuffers(1, &m_vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);

  Vertex::enableAll(0, 0, Vertex::bytesPerVertex());

  glBindVertexArray(0);
}

template<typename... Attribs>
OpenGLVertexBuffer<Attribs...>::~OpenGLVertexBuffer()
{
  if (m_vertexBuffer)
    glDeleteBuffers(1, &m_vertexBuffer);

  if (m_vertexArrayObject)
    glDeleteVertexArrays(1, &m_vertexArrayObject);
}

template<typename... Attribs>
void
OpenGLVertexBuffer<Attribs...>::allocate(size_t vertexCount, GLenum usage)
{
  assert(m_boundFlag);

  static_assert(sizeof(Vertex) == Vertex::bytesPerVertex());

  const size_t totalSize = vertexCount * Vertex::bytesPerVertex();

  glBufferData(GL_ARRAY_BUFFER, totalSize, nullptr, usage);
}

template<typename... Attribs>
void
OpenGLVertexBuffer<Attribs...>::write(size_t offset, const Vertex* vertices, size_t vertexCount)
{
  assert(m_boundFlag);

  static_assert(sizeof(Vertex) == Vertex::bytesPerVertex());

  const size_t byteOffset = Vertex::bytesPerVertex() * offset;

  const size_t byteCount = Vertex::bytesPerVertex() * vertexCount;

  glBufferSubData(GL_ARRAY_BUFFER, byteOffset, byteCount, vertices);
}

template<typename... Attribs>
void
OpenGLVertexBuffer<Attribs...>::bind()
{
  assert(!m_boundFlag);

  glBindVertexArray(m_vertexArrayObject);

  m_boundFlag = true;
}

template<typename... Attribs>
void
OpenGLVertexBuffer<Attribs...>::unbind()
{
  assert(m_boundFlag);

  glBindVertexArray(0);

  m_boundFlag = false;
}

template<typename... Attribs>
size_t
OpenGLVertexBuffer<Attribs...>::vertexCount()
{
  assert(m_boundFlag);

  GLint size = 0;

  glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

  return size_t(size) / Vertex::bytesPerVertex();
}

} // namespace Ak