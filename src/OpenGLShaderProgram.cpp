#include <Ak/OpenGLShaderProgram.h>

#include <glm/glm.hpp>

#include <cmrc/cmrc.hpp>

#include <array>
#include <sstream>

#include <cstring>

CMRC_DECLARE(AkShaders);

namespace Ak {

namespace {

template<GLenum Kind>
class Shader final
{
public:
  static std::string argToSource(const char* sourceArg)
  {
    if ((sourceArg[0] != ':') && (sourceArg[1] != '/'))
      return sourceArg;
    else
      return openSource(sourceArg + 2);
  }

  Shader(const char* sourceArg)
  {
    std::string source = argToSource(sourceArg);

    m_shaderID = glCreateShader(Kind);

    const GLchar* sourcePtr = source.c_str();

    GLint sourceSize = source.size();

    glShaderSource(m_shaderID, 1, &sourcePtr, &sourceSize);

    glCompileShader(m_shaderID);
  }

  ~Shader()
  {
    if (m_shaderID > 0u)
      glDeleteShader(m_shaderID);
  }

  bool isCompiled()
  {
    GLint success = 0;

    glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &success);

    return success != GL_FALSE;
  }

  std::string getInfoLog()
  {
    GLsizei infoLogLength = 0;

    glGetShaderiv(m_shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

    if (infoLogLength < 0)
      return std::string();

    std::string infoLog(size_t(infoLogLength), '\0');

    glGetShaderInfoLog(m_shaderID, infoLog.size(), &infoLogLength, &infoLog[0]);

    if (infoLogLength < 0)
      return std::string();

    infoLog.resize(size_t(infoLogLength));

    return infoLog;
  }

  GLuint getID() { return m_shaderID; }

private:
  static std::string openSource(const char* sourcePath)
  {
    auto fs = cmrc::AkShaders::get_filesystem();

    auto file = fs.open(sourcePath);

    return std::string(file.begin(), file.end());
  }

private:
  GLuint m_shaderID = 0;
};

} // namespace

OpenGLShaderProgram::OpenGLShaderProgram(const char* vertSourcePath, const char* fragSourcePath)
{
  Shader<GL_VERTEX_SHADER> vertShader(vertSourcePath);

  m_vertInfoLog = vertShader.getInfoLog();

  Shader<GL_FRAGMENT_SHADER> fragShader(fragSourcePath);

  m_fragInfoLog = fragShader.getInfoLog();

  if (!vertShader.isCompiled() || !fragShader.isCompiled())
    return;

  m_programID = glCreateProgram();

  glAttachShader(m_programID, vertShader.getID());
  glAttachShader(m_programID, fragShader.getID());

  glLinkProgram(m_programID);

  glDetachShader(m_programID, vertShader.getID());
  glDetachShader(m_programID, fragShader.getID());

  GLsizei infoLogLength = 0;

  glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);

  if (infoLogLength > 0) {

    m_linkInfoLog.resize(size_t(infoLogLength));

    glGetProgramInfoLog(m_programID, m_linkInfoLog.size(), &infoLogLength, &m_linkInfoLog[0]);

    if (infoLogLength < 0)
      m_linkInfoLog.clear();
    else
      m_linkInfoLog.resize(size_t(infoLogLength));
  }
}

OpenGLShaderProgram::~OpenGLShaderProgram()
{
  if (m_programID > 0u)
    glDeleteProgram(m_programID);
}

void
OpenGLShaderProgram::bind()
{
  assert(!m_boundFlag);

  glUseProgram(m_programID);

  m_boundFlag = true;
}

void
OpenGLShaderProgram::unbind()
{
  assert(m_boundFlag);

  glUseProgram(0);

  m_boundFlag = false;
}

bool
OpenGLShaderProgram::isLinked()
{
  if (!m_programID)
    return false;

  GLint linkStatus = 0;

  glGetProgramiv(m_programID, GL_LINK_STATUS, &linkStatus);

  return linkStatus != GL_FALSE;
}

std::string
OpenGLShaderProgram::getVertInfoLog() const
{
  return m_vertInfoLog;
}

std::string
OpenGLShaderProgram::getFragInfoLog() const
{
  return m_fragInfoLog;
}

std::string
OpenGLShaderProgram::getLinkInfoLog() const
{
  return m_linkInfoLog;
}

GLint
OpenGLShaderProgram::getUniformLocation(const char* name) const
{
  assert(m_boundFlag);

  return glGetUniformLocation(m_programID, name);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, int value)
{
  assert(m_boundFlag);

  glUniform1i(location, value);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, float value)
{
  assert(m_boundFlag);

  glUniform1f(location, value);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, const glm::vec2& value)
{
  assert(m_boundFlag);

  glUniform2f(location, value.x, value.y);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, const glm::vec3& value)
{
  assert(m_boundFlag);

  glUniform3f(location, value.x, value.y, value.z);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, const glm::vec4& value)
{
  assert(m_boundFlag);

  glUniform4f(location, value.x, value.y, value.z, value.w);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, const glm::mat2& value)
{
  assert(m_boundFlag);

  glUniformMatrix2fv(location, 1, GL_FALSE, &value[0][0]);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, const glm::mat3& value)
{
  assert(m_boundFlag);

  glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void
OpenGLShaderProgram::setUniformValue(GLint location, const glm::mat4& value)
{
  assert(m_boundFlag);
  glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

} // namespace Ak
