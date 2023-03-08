#version 460

layout (points) in;

layout (location = 0) in vec4 pPosition[];
layout (location = 1) in vec3 pNormal[];
layout (location = 2) in vec3 pColor[];
layout (location = 3) in vec2 pUV[];

layout (triangle_strip, max_vertices = 14) out;

layout (location = 0) out vec4 fragPosition;
layout (location = 1) out vec3 fragNormal;
layout (location = 2) out vec3 fragColor;
layout (location = 3) out vec2 fragUV;

layout (std430, push_constant) uniform PushConstants {
    mat4 mvp;
    vec4 octreeData;
} pushConstants;

void main() {
    float octreeDepth = pushConstants.octreeData.w;
    vec3 octreeSize = pushConstants.octreeData.xyz;
    vec3 voxelDimensions = vec3(
        octreeSize.x / pow(2, octreeDepth - 1),
        octreeSize.y / pow(2, octreeDepth - 1),
        octreeSize.z / pow(2, octreeDepth - 1)
    ) / 2.0f;

    vec4 inPosition = gl_in[0].gl_Position;

    // Account for Vulkan coordinate system
    inPosition.y = -inPosition.y;
    // inPosition.z = (inPosition.z + inPosition.w) / 2.0f;

    mat4 mvp = pushConstants.mvp;
    vec4 v0 = mvp * (inPosition + vec4(-voxelDimensions.x, -voxelDimensions.y,  voxelDimensions.z, 1.0f));
    vec4 v1 = mvp * (inPosition + vec4( voxelDimensions.x, -voxelDimensions.y,  voxelDimensions.z, 1.0f));
    vec4 v2 = mvp * (inPosition + vec4( voxelDimensions.x,  voxelDimensions.y,  voxelDimensions.z, 1.0f));
    vec4 v3 = mvp * (inPosition + vec4(-voxelDimensions.x,  voxelDimensions.y,  voxelDimensions.z, 1.0f));
    vec4 v4 = mvp * (inPosition + vec4(-voxelDimensions.x, -voxelDimensions.y, -voxelDimensions.z, 1.0f));
    vec4 v5 = mvp * (inPosition + vec4( voxelDimensions.x, -voxelDimensions.y, -voxelDimensions.z, 1.0f));
    vec4 v6 = mvp * (inPosition + vec4( voxelDimensions.x,  voxelDimensions.y, -voxelDimensions.z, 1.0f));
    vec4 v7 = mvp * (inPosition + vec4(-voxelDimensions.x,  voxelDimensions.y, -voxelDimensions.z, 1.0f));

    gl_Position = v3;
    fragPosition = v3;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v2;
    fragPosition = v2;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v0;
    fragPosition = v0;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v1;
    fragPosition = v1;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v5;
    fragPosition = v5;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v2;
    fragPosition = v2;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v6;
    fragPosition = v6;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v3;
    fragPosition = v3;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v7;
    fragPosition = v7;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v0;
    fragPosition = v0;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v4;
    fragPosition = v4;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v5;
    fragPosition = v5;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v7;
    fragPosition = v7;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    gl_Position = v6;
    fragPosition = v6;
    fragNormal = pNormal[0];
    fragColor = pColor[0];
    fragUV = pUV[0];
    EmitVertex();
    EndPrimitive();
}
