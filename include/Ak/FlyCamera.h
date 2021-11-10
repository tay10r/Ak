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

  vec3 position() const { return m_position; }

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

  void applyRelativeMove(vec3 relativeMove);

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

      m_camera->applyRelativeMove(worldSpaceMoveVector());
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

    void keyPressEvent(int key, int /* scancode */, int /* mods */) { keyEvent<true>(key); }

    void keyReleaseEvent(int key, int /* scancode */, int /* mods */) { keyEvent<false>(key); }

  private:
    vec3 worldSpaceMoveVector() const
    {
      const std::optional<vec2> moveVec = moveVector();
      if (!moveVec)
        return vec3(0, 0, 0);

      return glm::normalize((m_camera->forwardVector() * moveVec->x) + (m_camera->rightVector() * moveVec->y));
    }

    std::optional<vec2> moveVector() const
    {
      int x = 0;
      int y = 0;

      if (m_moveState[0])
        x += 1;

      if (m_moveState[1])
        y -= 1;

      if (m_moveState[2])
        x -= 1;

      if (m_moveState[3])
        y += 1;

      if ((x == 0) && (y == 0))
        return std::nullopt;
      else
        return glm::normalize(vec2(x, y));
    }

    template<bool IsPress>
    void keyEvent(int key)
    {
      switch (key) {
        case GLFW_KEY_W:
        case GLFW_KEY_UP:
          m_moveState[0] = IsPress;
          break;
        case GLFW_KEY_A:
        case GLFW_KEY_LEFT:
          m_moveState[1] = IsPress;
          break;
        case GLFW_KEY_S:
        case GLFW_KEY_DOWN:
          m_moveState[2] = IsPress;
          break;
        case GLFW_KEY_D:
        case GLFW_KEY_RIGHT:
          m_moveState[3] = IsPress;
          break;
      }
    }

  private:
    bool m_moveState[4]{ false, false, false, false };

    std::vector<std::chrono::high_resolution_clock::time_point> m_timePoints;

    std::optional<vec2> m_lastMousePosition;

    std::vector<vec2> m_movePoints;

    bool m_dragState = false;

    FlyCamera<Float>* m_camera;
  };

private:
  Float m_moveSpeed = 1;

  vec2 m_rotationSpeed = vec2(0.01 / Pi<Float>::value(), 0.01 / Pi<Float>::value());

  Float m_pitch = Pi<Float>::value() / 2;

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
glm::tvec3<Float>
FlyCamera<Float>::rightVector() const
{
  return glm::normalize(glm::cross(forwardVector(), upVector()));
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

template<typename Float>
void
FlyCamera<Float>::applyRelativeMove(vec3 relativeMove)
{
  m_position += relativeMove;
}

} // namespace Ak
