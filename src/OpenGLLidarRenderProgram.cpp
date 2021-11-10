#include <Ak/OpenGLLidarRenderProgram.h>

#include <Ak/OpenGLVertexBuffer.h>

#include <cassert>

#include <iostream>

namespace Ak {

OpenGLLidarRenderProgram::OpenGLLidarRenderProgram()
  : m_renderLidarProgram("shaders/render_lidar.vert", "shaders/render_lidar.frag")
  , m_normalEstimationProgram("shaders/render_lidar_normal_estimation.vert",
                              "shaders/render_lidar_normal_estimation.frag")
{
  m_renderLidarProgram.bind();

  m_mvpLocation = m_renderLidarProgram.getUniformLocation("mvp");

  m_renderLidarProgram.unbind();

  assert(m_mvpLocation >= 0);

  m_normalEstimationProgram.bind();

  m_eyeLocation = m_normalEstimationProgram.getUniformLocation("eye");

  m_normalEstimationProgram.unbind();

  m_framebuffer.bind();

  m_depthBuffer.bind();

  m_framebuffer.attach(m_depthBuffer);

  m_depthBuffer.unbind();

  m_positionIntensityTexture.bind();

  m_positionIntensityTexture.setMinMagFilters(GL_NEAREST, GL_NEAREST);

  m_framebuffer.attach(m_positionIntensityTexture);

  m_positionIntensityTexture.unbind();

  m_framebuffer.unbind();
}

bool
OpenGLLidarRenderProgram::isInitialized()
{
  m_framebuffer.bind();

  bool isFramebufferComplete = m_framebuffer.isComplete();

  m_framebuffer.unbind();

  return isFramebufferComplete;
}

void
OpenGLLidarRenderProgram::setMVP(const glm::mat4& mvp)
{
  m_renderLidarProgram.bind();

  m_renderLidarProgram.setUniformValue(m_mvpLocation, mvp);

  m_renderLidarProgram.unbind();
}

void
OpenGLLidarRenderProgram::setEye(const glm::vec3& eye)
{
  m_normalEstimationProgram.bind();

  m_normalEstimationProgram.setUniformValue(m_eyeLocation, eye);

  m_normalEstimationProgram.unbind();
}

void
OpenGLLidarRenderProgram::render(const OpenGLVertexBuffer<glm::vec3, float>& lidarPoints)
{
  assert(lidarPoints.isBound());

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_PROGRAM_POINT_SIZE);

  // Pass 1 : Project point positions and intensities onto texture.

  GLint dims[4]{ 0, 0, 0, 0 };

  glGetIntegerv(GL_VIEWPORT, dims);

  const GLint w = dims[2];
  const GLint h = dims[3];

  m_positionIntensityTexture.bind();

  m_positionIntensityTexture.resize(w, h, GL_RGBA32F, GL_RGBA, GL_FLOAT);

  m_positionIntensityTexture.unbind();

  m_depthBuffer.bind();

  m_depthBuffer.resize(w, h);

  m_depthBuffer.unbind();

  m_framebuffer.bind();

  m_renderLidarProgram.bind();

  GLfloat originalClearColor[4];

  glGetFloatv(GL_COLOR_CLEAR_VALUE, originalClearColor);

  // TODO: This statement below is used to initialize the texture containing the lidar points to zero, so that a texel
  //       can be tested to see if it actually has a lidar point (values of zero indicate no point is present). Use the
  //       stencil buffer instead to discard fragments that have no lidar point.
  glClearColor(0, 0, 0, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(originalClearColor[0], originalClearColor[1], originalClearColor[2], originalClearColor[3]);

  glDrawArrays(GL_POINTS, 0, lidarPoints.getVertexCount());

  m_renderLidarProgram.unbind();

  m_framebuffer.unbind();

  // Pass 2 : Estimate normals of each point in screen space and compute lighting

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_positionIntensityTexture.bind();

  m_normalEstimationProgram.bind();

  m_normalEstimationProgram.bindQuad();

  glDrawArrays(GL_TRIANGLES, 0, 6);

  m_normalEstimationProgram.unbindQuad();

  m_normalEstimationProgram.unbind();

  m_positionIntensityTexture.unbind();

  glDisable(GL_DEPTH_TEST);

  glDisable(GL_PROGRAM_POINT_SIZE);
}

} // namespace Ak
