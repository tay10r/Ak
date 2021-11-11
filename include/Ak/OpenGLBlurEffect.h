#pragma once

#include <Ak/OpenGLScreenSpaceEffect.h>

namespace Ak {

class OpenGLTexture2D;

class OpenGLBlurEffect : public OpenGLScreenSpaceEffect
{
public:
  OpenGLBlurEffect()
    : OpenGLScreenSpaceEffect(":/shaders/blur_effect.vert", ":/shaders/blur_effect.frag")
  {}

  void render(OpenGLTexture2D& colorTexture);
};

} // namespace Ak
