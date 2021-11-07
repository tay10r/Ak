#version 430 core

#define RADIUS 8

layout(location = 0) in vec2 textureCoords;

uniform sampler2D colorTexture;

layout(location = 0) out vec4 outColor;

void
main()
{
  ivec2 s = textureSize(colorTexture, 0);

  vec2 pixelSize = vec2(1.0 / float(s.x), 1.0 / float(s.y));

  vec2 textureCoordsMin = textureCoords - (pixelSize * float(RADIUS));

  vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);

  for (int y = 0; y < (RADIUS * 2); y++) {

    for (int x = 0; x < (RADIUS * 2); x++) {

      sum += texture(colorTexture, textureCoordsMin + vec2(float(x) * pixelSize.x, float(y) * pixelSize.y));
    }
  }

  vec4 avg = sum * (1.0 / float(RADIUS * RADIUS * 4));

  outColor = avg;
}
