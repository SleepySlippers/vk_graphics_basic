#version 450

layout(location = 0) out vec2 out_fragColor;

void main() {
    out_fragColor = vec2(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z);
}