#pragma once

#include <glad/glad.h>

#include <glm/fwd.hpp>

#include <string>

namespace Ak {

class OpenGLShaderProgram
{
public:
  OpenGLShaderProgram(const char* vertShader, const char* fragShader);

  OpenGLShaderProgram(const OpenGLShaderProgram&) = delete;

  virtual ~OpenGLShaderProgram();

  void bind();

  void unbind();

  bool isBound() const { return m_boundFlag; }

  bool isLinked();

  std::string getVertInfoLog() const;

  std::string getFragInfoLog() const;

  std::string getLinkInfoLog() const;

  GLint getUniformLocation(const char* name) const;

  void setUniformValue(GLint location, int value);

  void setUniformValue(GLint location, float value);

  void setUniformValue(GLint location, const glm::vec2& value);

  void setUniformValue(GLint location, const glm::vec3& value);

  void setUniformValue(GLint location, const glm::vec4& value);

  void setUniformValue(GLint location, const glm::mat2& value);

  void setUniformValue(GLint location, const glm::mat3& value);

  void setUniformValue(GLint location, const glm::mat4& value);

private:
  GLuint m_programID = 0;

  std::string m_vertInfoLog;

  std::string m_fragInfoLog;

  std::string m_linkInfoLog;

  bool m_boundFlag = false;
};

template<typename Derived>
class OpenGLShaderProgramTemplate : public OpenGLShaderProgram
{
public:
  OpenGLShaderProgramTemplate(const char* vertShader, const char* fragShader)
    : OpenGLShaderProgram(vertShader, fragShader)
  {}

  virtual ~OpenGLShaderProgramTemplate() = default;
};

} // namespace Ak
