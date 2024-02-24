#version 450

layout (triangles) in;
layout (triangle_strip, max_vertices = 32) out;

layout(push_constant) uniform params_t
{
    mat4 mProjView;
    mat4 mModel;
    float iTime;
} params;

layout(location = 0) in VS_IN
{
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;
} vIn[];

layout(location = 0) out VS_OUT
{
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;
} vOut;

void main()
{

  float t = max(0.0f, sin(params.iTime)) / 10.;
  vec3 normAverage = normalize(vIn[0].wNorm + vIn[1].wNorm + vIn[2].wNorm);
  vec3 normal = normAverage * t;

  vec3 rot = normalize(cross(normAverage, vec3(0., 0., 1.))) * t;

  for (int i = 0; i < 3; ++i)
  {
    vOut.wPos     = vIn[i].wPos;
    vOut.wNorm    = vIn[i].wNorm;
    vOut.wTangent = vIn[i].wTangent;
    vOut.texCoord = vIn[i].texCoord;

    gl_Position = params.mProjView * vec4(vIn[i].wPos + normal + rot, 1.0);    

    EmitVertex();
  }

  EndPrimitive();
}