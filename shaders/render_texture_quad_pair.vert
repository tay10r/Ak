#version 430 core

layout(location = 0) uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec2 position;

layout(location = 0) out vec2 texCoords;

void
main()
{
  texCoords = position;

  vec4 p = vec4((position.x * 2.0) - 1.0, (position.y * 2.0) - 1.0, 0.0, 1.0);

  gl_Position = mvp * p;
}
