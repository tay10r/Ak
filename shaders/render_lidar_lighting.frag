#version 430 core

#define RADIUS 8

#define PI 3.1415

layout(location = 0) in vec2 texCoords;

layout(location = 0) out vec4 outColor;

layout(location = 1) uniform vec3 eye = vec3(0.0, 0.0, 0.0);

layout(location = 2) uniform float maxSquaredDistance = 1.0;

uniform sampler2D positionIntensityTexture;

vec2 xy2uv(ivec2 xy, ivec2 texSize)
{
  return vec2(xy.x / float(texSize.x), xy.y / float(texSize.y));
}

ivec2 uv2xy(vec2 uv, ivec2 texSize)
{
  return ivec2(round(uv.x * texSize.x), round(uv.y * texSize.y));
}

vec3 estimatePointNormal(vec4 centerTexel)
{
  const ivec2 texSize = textureSize(positionIntensityTexture, 0);

  const ivec2 xyCenter = uv2xy(texCoords, texSize);

  const ivec2 xyMin = xyCenter - RADIUS;
  const ivec2 xyMax = xyCenter + RADIUS;

  vec4 nearest[8];

  float nearestSquaredDistances[8] = float[](-1, -1, -1, -1, -1, -1, -1, -1);

  for (int y = xyMin.y; y <= xyMax.y; y++) {

    for (int x = xyMin.x; x <= xyMax.x; x++) {

      if ((x == xyCenter.x) && (y == xyCenter.y))
        continue;

      const vec2 neighborCoords = xy2uv(ivec2(x, y), texSize);

      const vec4 neighbor = texture(positionIntensityTexture, neighborCoords);

      if (neighbor.xyz == eye)
        continue;

      const vec3 deltaPos = neighbor.xyz - centerTexel.xyz;

      const float squaredDistance = dot(deltaPos, deltaPos);

      if (squaredDistance > maxSquaredDistance)
        continue;

      const float xLocal = x - xyCenter.x;
      const float yLocal = y - xyCenter.y;

      const vec2 circleCoord = normalize(vec2(xLocal, yLocal));

      const float angle = atan(circleCoord.y, circleCoord.x) + PI;

      const int neighborIndex = clamp(int((angle / (2 * PI)) * 8), 0, 7);

      if ((nearestSquaredDistances[neighborIndex] < 0) || (squaredDistance < nearestSquaredDistances[neighborIndex])) {

        nearestSquaredDistances[neighborIndex] = squaredDistance;

        nearest[neighborIndex] = neighbor;
      }
    }
  }

  /* TODO : Make everything above a function, so that we can use it in other shaders. */

  vec3 edges[8];

  int edgeCount = 0;

  for (int i = 0; i < 8; i++) {
    if (nearestSquaredDistances[i] < 0)
      continue;
    edges[edgeCount] = nearest[i].xyz - centerTexel.xyz;
    edgeCount++;
  }

  vec3 avgNormal = vec3(0, 0, 0);

  if (edgeCount < 2) {
    return vec3(0, 0, 0);
  } else if (edgeCount == 2) {
    return normalize(cross(edges[1], edges[0]));
  } else {
    vec3 normalSum = vec3(0, 0, 0);
    for (int i = 0; i < edgeCount; i++) {
      normalSum += normalize(cross(edges[(i + 1) % edgeCount], edges[i]));
    }
    return normalize(normalSum * (1.0 / float(edgeCount + 1)));
  }
}

void
main()
{
  vec4 centerTexel = texture(positionIntensityTexture, texCoords);

  if (centerTexel.xyz == eye)
    outColor = vec4(0, 0, 0, 1);
  else
    outColor = vec4((estimatePointNormal(centerTexel) + 1.0) * 0.5, 1.0);
}
