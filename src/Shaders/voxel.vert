#version 450

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inUV;

layout (location = 0) out vec4 pPosition;
layout (location = 1) out vec3 pNormal;
layout (location = 2) out vec3 pColor;
layout (location = 3) out vec2 pUV;

void main() {
    gl_Position = inPosition;

    gl_PointSize = 1.0f;

    pPosition = inPosition;
    pColor = inColor;
    pUV = inUV;
    pNormal = inNormal;
}
