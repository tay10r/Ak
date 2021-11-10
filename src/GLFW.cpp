#include <Ak/GLFW.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Ak {

namespace {

void
glfwErrorCallback(int, const char* description)
{
#ifndef NDEBUG
  std::fprintf(stderr, "GLFW Error: %s\n", description);
#else
  (void)description;
#endif // NDEBUG
}

void
glErrorCallback(GLenum, GLenum, GLuint, GLenum, GLsizei length, const char* message, const void* windowTitlePtr)
{
#ifndef NDEBUG
  const char* windowTitle = windowTitlePtr ? ((const char*)windowTitlePtr) : "<unknown-window>";
  fprintf(stderr, "%s: GL Error: %.*s\n", windowTitle, (int)length, message);
#else
  (void)length;
  (void)message;
  (void)windowTitlePtr;
#endif
}

char*
copyString(const char* in)
{
  size_t len = std::strlen(in);

  char* copy = (char*)std::malloc(len + 1);

  std::memcpy(copy, in, len + 1);

  return copy;
}

void
framebufferResizeCallback(GLFWwindow* window, int w, int h)
{
  GLFWWindow* window_ = (GLFWWindow*)glfwGetWindowUserPointer(window);

  glViewport(0, 0, w, h);

  window_->notifyFramebufferResize(w, h);
}

void
cursorPosCallback(GLFWwindow* window, double x, double y)
{
  GLFWWindow* window_ = (GLFWWindow*)glfwGetWindowUserPointer(window);

  window_->notifyMouseMove(x, y);
}

void
mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
  GLFWWindow* window_ = (GLFWWindow*)glfwGetWindowUserPointer(window);

  if (action == GLFW_PRESS)
    window_->notifyMousePress(button, mods);
  else
    window_->notifyMouseRelease(button, mods);
}

void
keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  GLFWWindow* window_ = (GLFWWindow*)glfwGetWindowUserPointer(window);

  if (action == GLFW_PRESS)
    window_->notifyKeyPress(key, scancode, mods);
  else
    window_->notifyKeyRelease(key, scancode, mods);
}

} // namespace

bool
GLFW::init(MSAA msaa)
{
  if (glfwInit() != GLFW_TRUE)
    return false;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

  switch (msaa) {
    case MSAA::none:
      break;
    case MSAA::x2:
      glfwWindowHint(GLFW_SAMPLES, 2);
      break;
    case MSAA::x4:
      glfwWindowHint(GLFW_SAMPLES, 4);
      break;
    case MSAA::x8:
      glfwWindowHint(GLFW_SAMPLES, 8);
      break;
  }

  glfwSetErrorCallback(glfwErrorCallback);

  return true;
}

void
GLFW::pollEvents()
{
  glfwPollEvents();
}

void
GLFW::cleanup()
{
  glfwTerminate();
}

GLFWWindow::GLFWWindow(int w, int h, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
  : m_titleCopy(copyString(title))
  , m_self(glfwCreateWindow(w, h, m_titleCopy, monitor, share))
{
  glfwSetWindowUserPointer(m_self, this);

  glfwSetFramebufferSizeCallback(m_self, framebufferResizeCallback);

  glfwSetCursorPosCallback(m_self, cursorPosCallback);

  glfwSetMouseButtonCallback(m_self, mouseButtonCallback);

  glfwSetKeyCallback(m_self, keyCallback);

  if (m_self) {

    makeCurrent();

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glDebugMessageCallback(glErrorCallback, m_titleCopy);

    doneCurrent();
  }
}

GLFWWindow::~GLFWWindow()
{
  std::free(m_titleCopy);

  if (m_self)
    glfwDestroyWindow(m_self);
}

void
GLFWWindow::registerEventObserver(const std::shared_ptr<GLFWEventObserver>& observer)
{
  m_eventObservers.emplace_back(observer);
}

bool
GLFWWindow::makeCurrent()
{
  assert(m_self != nullptr);

  if (!m_self)
    return false;

  glfwMakeContextCurrent(m_self);

  return true;
}

bool
GLFWWindow::doneCurrent()
{
  assert(m_self != nullptr);

  if (!m_self)
    return false;

  glfwMakeContextCurrent(nullptr);

  return true;
}

float
GLFWWindow::aspectRatio() const
{
  int w = 0;
  int h = 0;
  glfwGetFramebufferSize(m_self, &w, &h);
  return float(w) / h;
}

void
GLFWWindow::fakeFramebufferResizeEvent()
{
  int w = 0;
  int h = 0;
  glfwGetFramebufferSize(m_self, &w, &h);

  notifyFramebufferResize(w, h);
}

void
GLFWWindow::notifyFramebufferResize(int w, int h)
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->resizeEvent(w, h);
}

void
GLFWWindow::notifyMouseMove(double x, double y)
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->mouseMoveEvent(x, y);
}

void
GLFWWindow::notifyMousePress(int button, int mods)
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->mousePressEvent(button, mods);
}

void
GLFWWindow::notifyMouseRelease(int button, int mods)
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->mouseReleaseEvent(button, mods);
}

void
GLFWWindow::notifyKeyPress(int key, int scancode, int mods)
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->keyPressEvent(key, scancode, mods);
}

void
GLFWWindow::notifyKeyRelease(int key, int scancode, int mods)
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->keyReleaseEvent(key, scancode, mods);
}

void
GLFWWindow::notifyAnimationFrame()
{
  for (std::shared_ptr<GLFWEventObserver>& eventObserver : m_eventObservers)
    eventObserver->animationFrameEvent();
}

} // namespace Ak
