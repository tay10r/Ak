#pragma once

#include <Ak/Constants.h>
#include <Ak/GLFW.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <optional>
#include <vector>

namespace Ak {

template<typename Float>
class FlyCamera final
{
public:
  using mat3 = glm::tmat3x3<Float>;

  using mat4 = glm::tmat4x4<Float>;

  using vec2 = glm::tvec2<Float>;

  using vec3 = glm::tvec3<Float>;

  using vec4 = glm::tvec4<Float>;

  vec3 upVector() const;

  vec3 forwardVector() const;

  vec3 rightVector() const;

  mat4 worldToCameraMatrix() const;

  mat4 cameraToWorldMatrix() const;

  mat3 cameraToWorldRotation() const;

  /// Creates a GLFW event proxy so that the user can control the camera with mouse input.
  ///
  /// @return A new event observer that can be registered with a window to receive input events as camera controls.
  std::shared_ptr<GLFWEventObserver> makeGLFWEventProxy();

  void applyMouseMotion(vec2 motion);

private:
  static vec3 sphericalToXYZ(Float pitch, Float yaw)
  {
    const Float x = std::cos(yaw) * std::sin(pitch);
    const Float y = std::sin(yaw) * std::sin(pitch);
    const Float z = std::cos(pitch);
    return vec3(y, z, -x);
  }

  class GLFWEventProxy final : public GLFWEventObserver
  {
  public:
    GLFWEventProxy(FlyCamera<Float>* camera)
      : m_camera(camera)
    {}

    void animationFrameEvent() override
    {
      if (m_lastMousePosition) {
        if (m_movePoints.size() < 2) {
          m_movePoints.emplace_back(m_lastMousePosition.value());
        } else {
          m_movePoints[0] = m_movePoints[1];
          m_movePoints[1] = m_lastMousePosition.value();
        }
      }

      if ((m_movePoints.size() > 1) && ((glm::length(m_movePoints[1] - m_movePoints[0]) > 0.00001))) {

        vec2 delta = m_movePoints[1] - m_movePoints[0];

        m_movePoints.erase(m_movePoints.begin());

        m_camera->applyMouseMotion(delta);
      }
    }

    void mousePressEvent(int button, int) override
    {
      if (button == GLFW_MOUSE_BUTTON_LEFT)
        m_dragState = true;
    }

    void mouseReleaseEvent(int button, int) override
    {
      if (button == GLFW_MOUSE_BUTTON_LEFT)
        m_dragState = false;
    }

    void mouseMoveEvent(double x, double y) { m_lastMousePosition = vec2(x, y); }

  private:
    std::vector<std::chrono::high_resolution_clock::time_point> m_timePoints;

    std::optional<vec2> m_lastMousePosition;

    std::vector<vec2> m_movePoints;

    bool m_dragState = false;

    FlyCamera<Float>* m_camera;
  };

private:
  vec2 m_rotationSpeed = vec2(0.01 / Pi<Float>::value(), 0.01 / Pi<Float>::value());

  Float m_pitch = 0;

  Float m_yaw = 0;

  vec3 m_position = vec3(0, 0, 0);
};

template<typename Float>
std::shared_ptr<GLFWEventObserver>
FlyCamera<Float>::makeGLFWEventProxy()
{
  return std::shared_ptr<GLFWEventObserver>(new GLFWEventProxy(this));
}

template<typename Float>
glm::tvec3<Float>
FlyCamera<Float>::forwardVector() const
{
  return sphericalToXYZ(m_pitch, m_yaw);
}

template<typename Float>
glm::tvec3<Float>
FlyCamera<Float>::upVector() const
{
  return sphericalToXYZ(m_pitch - (Pi<Float>::value() / 2), m_yaw);
}

template<typename Float>
glm::tmat4x4<Float>
FlyCamera<Float>::worldToCameraMatrix() const
{
  return glm::lookAt(m_position, m_position + forwardVector(), vec3(0, 1, 0));
}

template<typename Float>
void
FlyCamera<Float>::applyMouseMotion(vec2 motion)
{
  const Float pitchDelta = motion.y * m_rotationSpeed.y;

  const Float yawDelta = motion.x * m_rotationSpeed.x;

  m_pitch += pitchDelta;

  m_yaw += yawDelta;

  const Float epsilon = 0.000001;

  m_pitch = std::max(std::min(m_pitch, Pi<Float>::value() - epsilon), epsilon);
}

} // namespace Ak
