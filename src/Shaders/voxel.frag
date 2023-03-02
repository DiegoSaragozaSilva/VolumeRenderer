#version 460

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec2 inTexCoord;
layout (location = 3) in vec3 inNormal;
layout (location = 4) in vec3 inViewPosition;
layout (location = 5) in vec3 inViewDirection;

layout (location = 0) out vec4 outColor;

vec3 phongBRDF(vec3 lightDir, vec3 viewDir, vec3 normal, vec3 diffuseColor, vec3 specularColor, float shininess) {
    vec3 resultColor = diffuseColor;
    vec3 reflectDir = reflect(-lightDir, normal);
    float specDot = max(dot(reflectDir, viewDir), 0.0f);
    resultColor += pow(specDot, shininess) * specularColor;
    return resultColor;
}

void main() {
    vec3 lightDir = normalize(-inViewDirection);
    vec3 viewDir = normalize(-inViewPosition);
   
    vec3 lightColor = vec3(0.25f);
    vec3 specularColor = vec3(1.0f);
    vec3 ambientColor = vec3(0.01f);
    vec3 diffuseColor = inColor;
    vec3 radiance = ambientColor; 

    float shininess = 50.0f;
    float irradiancePerb = 1.0f;
    float irradiance = max(dot(lightDir, normalize(inNormal)), 0.0f) * irradiancePerb;
    if (irradiance > 0.0f) {
      vec3 brdf = phongBRDF(lightDir, viewDir, normalize(inNormal), diffuseColor, specularColor, shininess);
      radiance += brdf * irradiance * lightColor;
    }

    radiance = pow(radiance, vec3(1.0f / 2.2f));
    outColor = vec4(radiance, 1.0f);
}
