#pragma once

#include <Ak/OpenGLFramebuffer.h>
#include <Ak/OpenGLRenderbuffer.h>
#include <Ak/OpenGLShaderProgram.h>
#include <Ak/OpenGLTexture2D.h>

#include <glm/fwd.hpp>

namespace Ak {

template<typename... Attribs>
class OpenGLVertexBuffer;

/// This program is used to render a triangle mesh onto a set of textures suitable for hydrid rasterization and ray
/// tracing. It produces the following data:
///
/// - Albedo texture
/// - Normal texture
/// - Position texture
///
/// The albedo and normal textures are used for computing indirect light as well as for some denoising algorithms.
class OpenGLHRTMeshRenderProgram final : public OpenGLShaderProgram
{
public:
  OpenGLHRTMeshRenderProgram();

  /// Indicates if the framebuffer is initialized.
  ///
  /// @return True on success, false on failure.
  bool isInitialized();

  void setMVP(const glm::mat4& mvp);

  void render(const OpenGLVertexBuffer<glm::vec3, glm::vec3, glm::vec2>& vertexBuffer);

  void resizeFramebuffer(int w, int h);

  OpenGLTexture2D* albedoTexture() { return &m_albedoTexture; }

  OpenGLTexture2D* normalDepthTexture() { return &m_normalDepthTexture; }

private:
  GLint m_mvpLocation = -1;
  OpenGLFramebuffer m_framebuffer;
  OpenGLRenderbuffer m_renderbuffer;
  OpenGLTexture2D m_albedoTexture;
  OpenGLTexture2D m_normalDepthTexture;
};

} // namespace Ak
