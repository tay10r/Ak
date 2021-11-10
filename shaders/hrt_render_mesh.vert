#version 430 core

layout(location = 0) uniform mat4 mvp = mat4(1.0);

layout(location = 0) in vec3 position;

layout(location = 1) in vec3 normal;

layout(location = 2) in vec2 texCoords;

layout(location = 0) out vec4 normalDepthFromVertShader;

layout(location = 1) out vec2 texCoordsFromVertShader;

void
main()
{
  const vec4 p = mvp * vec4(position, 1.0);

  const vec3 n = (normal + 1.0) * 0.5;

  normalDepthFromVertShader = vec4(n, p.z);

  texCoordsFromVertShader = texCoords;

  gl_Position = p;
}
