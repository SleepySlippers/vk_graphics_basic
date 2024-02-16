#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 color;

layout (binding = 0) uniform sampler2D colorTex;

layout (location = 0 ) in VS_OUT
{
  vec2 texCoord;
} surf;

vec4 medianFilter(vec2 coord) {
  const int SZ = 3;
  const int HALF_SZ = SZ / 2;
  vec4 near[SZ * SZ];
  for (int i = -HALF_SZ; i <= HALF_SZ; ++i) {
    for (int j = -HALF_SZ; j <= HALF_SZ; ++j) {
      near[(i + HALF_SZ) * SZ + j + HALF_SZ] = textureLod(colorTex, coord + vec2(i, j), 0);
    }
  }

  for (int i = 0; i < SZ * SZ; ++i) {
    for (int j = i + 1; j < SZ * SZ; ++j) {
      vec4 mx = max(near[i], near[j]);
      near[i] = min(near[i], near[j]);
      near[j] = mx;
    }
  }

  return near[SZ * SZ / 2];
}

void main()
{
  //color = textureLod(colorTex, surf.texCoord, 0);
  //return;
  color = medianFilter(surf.texCoord);
}
