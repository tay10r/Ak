#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <vector>

namespace Ak {

class GLFW final
{
public:
  enum class MSAA
  {
    none,
    x2,
    x4,
    x8
  };

  static bool init(MSAA msaa = MSAA::none);

  static void cleanup();

  static void pollEvents();
};

class GLFWEventObserver
{
public:
  virtual ~GLFWEventObserver() = default;

  /// Called when the platform requests an animation frame. On destkop platforms, this is called at every iteration of
  /// the render loop. On the browser, this is called when the browser decides is appropriate.
  virtual void animationFrameEvent() {}

  virtual void resizeEvent(int /* width */, int /* height */) {}

  virtual void mouseMoveEvent(double /* x */, double /* y */) {}

  virtual void mousePressEvent(int /* button */, int /* mods */) {}

  virtual void mouseReleaseEvent(int /* button */, int /* mods */) {}
};

class GLFWWindow final
{
public:
  GLFWWindow(int w, int h, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

  GLFWWindow(const GLFWWindow&) = delete;

  ~GLFWWindow();

  void registerEventObserver(const std::shared_ptr<GLFWEventObserver>&);

  bool isCreated() const noexcept { return m_self != nullptr; }

  bool makeCurrent();

  bool doneCurrent();

  /// Uses the current framebuffer width and height and notifies the event observers as if it was just changed to that
  /// size. This can be helpful when used to initialize data that depends on the width and height of the window.
  void fakeFramebufferResizeEvent();

  operator GLFWwindow*() noexcept { return m_self; }

  operator const GLFWwindow*() const noexcept { return m_self; }

  /// @note Only meant for internal use.
  void notifyFramebufferResize(int w, int h);

  /// @note Only meant for internal use.
  void notifyMouseMove(double, double);

  /// @note Only meant for internal use.
  void notifyMousePress(int, int);

  /// @note Only meant for internal use.
  void notifyMouseRelease(int, int);

  /// @note Only meant for internal use.
  void notifyAnimationFrame();

private:
  char* m_titleCopy = nullptr;

  GLFWwindow* m_self = nullptr;

  std::vector<std::shared_ptr<GLFWEventObserver>> m_eventObservers;
};

} // namespace Ak
