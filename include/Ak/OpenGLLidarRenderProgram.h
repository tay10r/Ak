#pragma once

#include <Ak/OpenGLFramebuffer.h>
#include <Ak/OpenGLRenderbuffer.h>
#include <Ak/OpenGLScreenSpaceEffect.h>
#include <Ak/OpenGLShaderProgram.h>
#include <Ak/OpenGLTexture2D.h>

#include <glm/fwd.hpp>

namespace Ak {

template<typename... Attribs>
class OpenGLVertexBuffer;

class OpenGLLidarRenderProgram final
{
public:
  OpenGLLidarRenderProgram();

  bool isInitialized();

  void setEye(const glm::vec3& eye);

  void setMVP(const glm::mat4& mvp);

  void render(const OpenGLVertexBuffer<glm::vec3, float>& buffer);

private:
  OpenGLRenderbuffer m_depthBuffer;

  OpenGLTexture2D m_positionIntensityTexture;

  OpenGLFramebuffer m_framebuffer;

  OpenGLShaderProgram m_renderLidarProgram;

  OpenGLScreenSpaceEffect m_normalEstimationProgram;

  /// The MVP location for the point rendering program.
  GLint m_mvpLocation = -1;

  /// The camera eye location for the lighting rendering program.
  GLint m_eyeLocation = -1;
};

} // namespace Ak
