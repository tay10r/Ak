#version 430 core

layout(location = 0) uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec3 position;

layout(location = 1) in vec4 pointColor;

layout(location = 0) out vec4 fragColor;

void
main()
{
  fragColor = pointColor;

  gl_Position = mvp * vec4(position, 1.0);
}
