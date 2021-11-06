#pragma once

#include <glad/glad.h>

#include <string>

namespace Ak {

class OpenGLShaderProgram
{
public:
  OpenGLShaderProgram(const char* vertShader, const char* fragShader);

  OpenGLShaderProgram(const OpenGLShaderProgram&) = delete;

  virtual ~OpenGLShaderProgram();

  void bind();

  static void unbind();

  bool isLinked();

  std::string getVertInfoLog() const;

  std::string getFragInfoLog() const;

  std::string getLinkInfoLog() const;

private:
  GLuint m_programID = 0;

  std::string m_vertInfoLog;

  std::string m_fragInfoLog;

  std::string m_linkInfoLog;
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
