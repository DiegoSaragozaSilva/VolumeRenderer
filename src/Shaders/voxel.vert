#version 450

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec4 inColor;
layout (location = 3) in vec2 inUV;

void main() {
    gl_Position = inPosition;
    gl_Position.y *= -1;
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;
}
