#version 460

layout (location = 0) in vec4 fragPosition;
layout (location = 1) in vec3 fragNormal;
layout (location = 2) in vec3 fragColor;
layout (location = 3) in vec2 fragTexCoord;

layout (location = 0) out vec4 outColor;

layout (std430, push_constant) uniform PushConstants {
    layout (offset = 80) vec4 viewMatrix;
    vec4 viewPosition;
    vec4 viewDirection;
} pushConstants;

void main() {
    vec3 viewNormal = vec3(normalize(pushConstants.viewMatrix * vec4(fragNormal, 1.0f)));
    vec3 viewDirection = vec3(normalize(pushConstants.viewMatrix * pushConstants.viewDirection));

    vec3 lightColor = vec3(0.025f);
    vec3 lightPosition = pushConstants.viewPosition.xyz;
    vec3 lightDirection = viewDirection;

    float ambientStrength = 0.01f;
    vec3 ambientLight = ambientStrength * lightColor; 

    float intensity = max(dot(viewNormal, lightDirection), 0.0);
    vec3 diffuseLight = intensity * lightColor;

    vec3 finalColor = max(ambientLight, diffuseLight) * fragColor;
    outColor = vec4(finalColor, 1.0f);
}
