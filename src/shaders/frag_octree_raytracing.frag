#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler3D texSampler;

void main() {
    vec4 cellData = texture(texSampler, vec3((fragColor.xy + 1.0) / 2.0, 0));
    outColor = vec4(cellData.xyz, 0);
}
