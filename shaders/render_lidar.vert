#version 430 core

layout(location = 0) uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec3 position;

layout(location = 1) in float intensity;

layout(location = 0) out vec4 positionIntensityTuple;

void
main()
{
  positionIntensityTuple = vec4(position, intensity);

  gl_Position = mvp * vec4(position, 1.0);
}
