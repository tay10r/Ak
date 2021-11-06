#include <Ak/SingleWindowGLFWApp.h>

#include <Ak/GLFW.h>

#include <memory>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace Ak {

SingleWindowGLFWApp::~SingleWindowGLFWApp() = default;

const char*
SingleWindowGLFWApp::title() const noexcept
{
  return nullptr;
}

int
run(int argc, char** argv, SingleWindowGLFWApp::FactoryMethod factoryMethod)
{
  std::vector<char*> appArgs;

  appArgs.emplace_back(argv[0]);

  for (int i = 1; i < argc; i++) {

    if (std::strcmp(argv[i], "--") == 0) {

      for (int j = i; j < argc; j++)
        appArgs.emplace_back(argv[j]);

    } else {

      std::fprintf(stderr, "Unknown option '%s'\n", argv[i]);

      return EXIT_FAILURE;
    }
  }

  if (!GLFW::init())
    return EXIT_FAILURE;

  {
    const int w = 640;
    const int h = 480;

    GLFWWindow window(w, h, argv[0]);

    window.makeCurrent();

    glfwSwapInterval(1);

    {
      std::unique_ptr<SingleWindowGLFWApp> app(factoryMethod(int(appArgs.size()), &appArgs[0], window));

      const char* title = app->title();
      if (title)
        glfwSetWindowTitle(window, title);

      while (!glfwWindowShouldClose(window)) {

        app->requestAnimationFrame(window);

        glfwSwapBuffers(window);

        GLFW::pollEvents();
      }
    }

    window.doneCurrent();
  }

  GLFW::cleanup();

  return EXIT_SUCCESS;
}

} // namespace Ak
