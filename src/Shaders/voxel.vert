#version 450

layout (std430, push_constant) uniform PushConstants {
    vec4 viewPosition;
    vec4 viewDirection;
    mat4 mvp;
} pushConstants;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inTexCoord;

layout (location = 0) out vec3 outColor;

void main() {
    gl_Position = pushConstants.mvp * vec4(inPosition, 1.0f);

    // Account for Vulkan coordinate system
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;

    gl_PointSize = 5.0f;

    outColor = inColor;
}
