#version 430 core

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

uniform sampler2D positionIntensityTexture;

void
main()
{
  outColor = texture(positionIntensityTexture, texCoords);
}
