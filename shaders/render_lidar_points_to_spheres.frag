#version 430 core

#define RADIUS 4

#define PERIMETER ((RADIUS * 2) + 1)

#define PI 3.1415926535

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(location = 1) uniform vec3 eye = vec3(0.0, 0.0, 0.0);

uniform sampler2D positionIntensityTexture;

uniform sampler2D normalTexture;

void
main()
{
  const ivec2 texSize = textureSize(positionIntensityTexture, 0);

  const vec2 texelSize = vec2(1, 1) / vec2(texSize.x, texSize.y);

  const vec2 xyMin = texCoords - (texelSize * float(RADIUS));
  const vec2 xyMax = texCoords + (texelSize * float(RADIUS));

  for (int y = 0; y < PERIMETER; y++) {

    for (int x = 0; x < PERIMETER; x++) {

      const vec2 texelCoords = mix(xyMin, xyMax, vec2(float(x) / float(PERIMETER - 1), float(y) / float(PERIMETER - 1)));

      const vec4 neighbor = texture(positionIntensityTexture, texelCoords);

      if (neighbor.xyz == vec3(0, 0, 0))
        continue;
    }
  }

  outColor = vec4(1, 1, 1, 1);
}
