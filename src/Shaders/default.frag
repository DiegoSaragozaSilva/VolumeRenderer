#version 460

layout (binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;

layout (location = 0) out vec4 outColor;

void main() {
    // float ambient = 0.1f;
    // float diffuse = max(dot(inNormal, normalize(vec3(1.0f, 1.0f, -12.0f))), 0.0f);
    // vec3 shadedColor = (ambient + diffuse) * inColor;
    // outColor = vec4(shadedColor, 1.0f);

    outColor = texture(texSampler, inTexCoord);
}
