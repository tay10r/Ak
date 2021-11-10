#include <Ak/OpenGLBlurEffect.h>

#include <Ak/OpenGLTexture2D.h>

namespace Ak {

void
OpenGLBlurEffect::render(OpenGLTexture2D& texture)
{
  (void)texture;

  assert(isBound());

  assert(texture.isBound());

  bindQuad();

  glDrawArrays(GL_TRIANGLES, 0, 6);

  unbindQuad();
}

} // namespace Ak
