#version 430 core

layout(location = 0) in vec2 textureCoords;

uniform sampler2D colorTexture;

layout(location = 0) out vec4 outColor;

void
main()
{
  outColor = texture(colorTexture, textureCoords);
}
