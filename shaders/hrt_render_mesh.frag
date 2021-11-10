#version 430 core

layout(location = 0) in vec4 normalDepthFromVertShader;

layout(location = 1) in vec2 texCoordsFromVertShader;

layout(location = 0) out vec3 outAlbedo;

layout(location = 1) out vec4 outNormalDepth;

uniform sampler2D albedoTexture;

void
main()
{
  outAlbedo = texture(albedoTexture, texCoordsFromVertShader).rgb;

  outNormalDepth = vec4(normalDepthFromVertShader.xyz, (gl_FragCoord.z - 0.5) * 2);
}
