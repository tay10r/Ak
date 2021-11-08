#include <Ak/SingleWindowGLFWApp.h>

#include <Ak/GLFW.h>

#include <memory>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Ak {

int
run(int argc, char** argv, SingleWindowGLFWApp::FactoryMethod factoryMethod)
{
  std::vector<char*> appArgs;

  appArgs.emplace_back(argv[0]);

  GLFW::MSAA msaa = GLFW::MSAA::none;

  for (int i = 1; i < argc; i++) {

    if (std::strcmp(argv[i], "--") == 0) {

      for (int j = i + 1; j < argc; j++)
        appArgs.emplace_back(argv[j]);

      break;
    }

    if (std::strcmp(argv[i], "--msaa=2") == 0) {
      msaa = GLFW::MSAA::x2;
    } else if (std::strcmp(argv[i], "--msaa=4") == 0) {
      msaa = GLFW::MSAA::x4;
    } else if (std::strcmp(argv[i], "--msaa=8") == 0) {
      msaa = GLFW::MSAA::x8;
    } else if (std::strcmp(argv[i], "--msaa") == 0) {
      msaa = GLFW::MSAA::x4;
    } else {
      std::fprintf(stderr, "Unknown option '%s'\n", argv[i]);
      return EXIT_FAILURE;
    }
  }

  if (!GLFW::init(msaa))
    return EXIT_FAILURE;

  bool success = true;

  {
    const int w = 640;
    const int h = 480;

    GLFWWindow window(w, h, argv[0]);

    window.makeCurrent();

    if (msaa != GLFW::MSAA::none)
      glEnable(GL_MULTISAMPLE);

    glfwSwapInterval(1);

    {
      std::unique_ptr<SingleWindowGLFWApp> app(factoryMethod(int(appArgs.size()), &appArgs[0], window));
      if (!app)
        success = false;

      const char* title = app ? app->title() : nullptr;
      if (title)
        glfwSetWindowTitle(window, title);

      if (app) {
        while (!glfwWindowShouldClose(window)) {

          window.notifyAnimationFrame();

          app->requestAnimationFrame(window);

          glfwSwapBuffers(window);

          GLFW::pollEvents();
        }
      }
    }

    if (msaa != GLFW::MSAA::none)
      glDisable(GL_MULTISAMPLE);

    window.doneCurrent();
  }

  GLFW::cleanup();

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

} // namespace Ak
