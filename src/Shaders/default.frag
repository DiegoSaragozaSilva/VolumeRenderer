#version 460

layout (location = 0) in vec4 fragPosition;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec3 fragColor;
layout (location = 3) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

layout (binding = 0) uniform sampler2D inTexture;

layout (std430, push_constant) uniform PushConstants {
    layout (offset = 64) vec4 viewPosition;
    vec4 viewDirection;
} pushConstants;

void main() {
    vec3 lightColor = vec3(0.025f);
    vec3 lightPosition = pushConstants.viewPosition.xyz;
    vec3 lightDirection = normalize(lightPosition - fragPosition.xyz);

    float ambientStrength = 0.01f;
    vec3 ambientLight = ambientStrength * lightColor; 

    float diffuse = max(dot(fragNormal, lightDirection), 0.0);
    vec3 diffuseLight = diffuse * lightColor;

    vec3 finalColor = (ambientLight + diffuseLight) * fragColor;
    outColor = vec4(finalColor, 1.0f);
}


