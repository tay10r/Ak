#pragma once

namespace Ak {

class GLFWWindow;

class SingleWindowGLFWApp
{
public:
  using FactoryMethod = SingleWindowGLFWApp* (*)(int argc, char** argv, GLFWWindow& window);

  virtual ~SingleWindowGLFWApp() = default;

  virtual void requestAnimationFrame(GLFWWindow& window) = 0;

  virtual const char* title() const noexcept { return nullptr; }
};

int
run(int argc, char** argv, SingleWindowGLFWApp::FactoryMethod factoryMethod);

} // namespace Ak
