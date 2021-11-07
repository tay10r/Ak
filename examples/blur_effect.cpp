#include <Ak/GLFW.h>
#include <Ak/OpenGLBlurEffect.h>
#include <Ak/OpenGLTexture2D.h>
#include <Ak/SingleWindowGLFWApp.h>

#include <glm/glm.hpp>

#include <random>
#include <vector>

#include <cstdio>
#include <cstdlib>

namespace {

class App final : public Ak::SingleWindowGLFWApp
{
public:
  App(Ak::OpenGLTexture2D&& texture)
    : m_texture(std::move(texture))
  {}

  const char* title() const noexcept override { return "Blur Effect"; }

  void requestAnimationFrame(Ak::GLFWWindow&) override
  {
    glClear(GL_COLOR_BUFFER_BIT);

    m_texture.bind();

    m_blurEffect.bind();

    m_blurEffect.render(m_texture);

    m_blurEffect.unbind();

    m_texture.unbind();
  }

private:
  Ak::OpenGLTexture2D m_texture;

  Ak::OpenGLBlurEffect m_blurEffect;
};

static Ak::SingleWindowGLFWApp*
makeApp(int argc, char** argv, Ak::GLFWWindow& window)
{
  if (argc < 2) {
    std::fprintf(stderr, "usage: %s <image>\n", argv[0]);
    return nullptr;
  }

  const char* imagePath = argv[1];

  Ak::OpenGLTexture2D texture;

  texture.bind();

  if (!texture.openFile(imagePath)) {
    texture.unbind();
    std::fprintf(stderr, "%s: failed to open '%s'\n", argv[0], imagePath);
    return nullptr;
  }

  texture.unbind();

  return new App(std::move(texture));
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
