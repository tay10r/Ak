#pragma once

#include <Ak/GLFW.h>

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>

namespace Ak {

template<typename Float>
class FlyCamera final
{
public:
  using mat4 = glm::tmat4x4<Float>;

  using vec3 = glm::tvec3<Float>;

  using vec4 = glm::tvec4<Float>;

  class GLFWEventProxy final : public GLFWEventObserver
  {
  public:
    void resizeEvent(int, int) override {}
  };

  mat3 cameraToWorldRotation() const;

private:
  vec3 m_position;
};

} // namespace Ak
