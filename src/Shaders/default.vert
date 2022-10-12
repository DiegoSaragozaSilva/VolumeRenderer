#version 460

layout (push_constant) uniform PushConstants {
    mat4 mvp;
} pushConstants;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec3 inColor;
layout (location = 3) in vec2 inTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoord;
layout (location = 2) out vec3 outNormal;

void main() {
    gl_Position = pushConstants.mvp * vec4(inPosition, 1.0f);

    // Account for Vulkan coordinate system
    gl_Position.y = -gl_Position.y;
    gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0f;

    outColor = inColor;
    outTexCoord = inTexCoord;
    outNormal = inNormal;
}
