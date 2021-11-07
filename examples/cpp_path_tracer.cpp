#include <Ak/GLFW.h>
#include <Ak/OpenGLTexture2D.h>
#include <Ak/OpenGLTextureQuadPair.h>
#include <Ak/SingleWindowGLFWApp.h>

#include <glm/glm.hpp>

#include <random>
#include <vector>

#include <cstdio>
#include <cstdlib>

namespace {

struct Framebuffer final
{
  Ak::OpenGLTexture2D colorTexture;

  std::vector<glm::vec3> rgb;

  std::vector<std::minstd_rand> rng;

  int width = 0;

  int height = 0;

  int size() const noexcept { return width * height; }
};

class FramebufferResizer final : public Ak::GLFWEventObserver
{
public:
  FramebufferResizer(const std::shared_ptr<Framebuffer>& framebuffer)
    : m_framebuffer(framebuffer)
  {}

  void resizeEvent(int w, int h) override
  {
    m_framebuffer->width = w;

    m_framebuffer->height = h;

    m_framebuffer->rgb.resize(w * h);

    m_framebuffer->rng.resize(w * h);

    std::seed_seq seed{ w, h, 1234 };

    std::mt19937 seedRng(seed);

    for (int i = 0; i < m_framebuffer->rng.size(); i++)
      m_framebuffer->rng[i] = std::minstd_rand(seedRng());

    m_framebuffer->colorTexture.bind();

    m_framebuffer->colorTexture.resize(w, h, GL_RGB, GL_FLOAT);

    m_framebuffer->colorTexture.unbind();
  }

private:
  std::shared_ptr<Framebuffer> m_framebuffer;
};

class App final : public Ak::SingleWindowGLFWApp
{
public:
  App(Ak::GLFWWindow& window)
  {
    std::shared_ptr<Ak::GLFWEventObserver> framebufferResizer(new FramebufferResizer(m_framebuffer));

    window.registerEventObserver(framebufferResizer);

    window.fakeFramebufferResizeEvent();

    glClearColor(0, 0, 0, 1);
  }

  const char* title() const noexcept override { return "C++ Path Tracer"; }

  void requestAnimationFrame(Ak::GLFWWindow&) override
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int i = 0; i < m_framebuffer->size(); i++) {

      const int x = i % m_framebuffer->width;
      const int y = i / m_framebuffer->width;

      const float u = (x + 0.5f) / m_framebuffer->width;
      const float v = (y + 0.5f) / m_framebuffer->height;

      m_framebuffer->rgb[i] = glm::vec3(u, v, 1);
    }

    m_framebuffer->colorTexture.bind();

    m_framebuffer->colorTexture.write(0, 0, m_framebuffer->width, m_framebuffer->height, m_framebuffer->rgb.data());

    m_framebuffer->colorTexture.unbind();

    m_textureQuadProgram.render(m_textureQuad);
  }

private:
  std::shared_ptr<Framebuffer> m_framebuffer{ new Framebuffer() };

  Ak::OpenGLTextureQuadPair m_textureQuad{ &m_framebuffer->colorTexture };

  Ak::OpenGLTextureQuadPair::RenderProgram m_textureQuadProgram;
};

static Ak::SingleWindowGLFWApp*
makeApp(int, char**, Ak::GLFWWindow& window)
{
  return new App(window);
}

} // namespace

int
main(int argc, char** argv)
{
  return Ak::run(argc, argv, &makeApp);
}
