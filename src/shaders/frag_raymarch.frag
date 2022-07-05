#version 450

#define M_PI 3.1415926535897932384626433832795
#define MAX_DIST 100.0
#define MIN_DIST 0.0
#define EPSILON 0.0001
#define MAX_STEPS 1000
#define SPHERE_RADIUS 0.5

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBuffer {
    int windowWidth;
    int windowHeight;
    float time;
} ubo;

vec3 getRayDirection(vec2 windowSize, vec2 coord, float fov) {
    vec2 xy = coord.xy - windowSize / 2.0;
    float z = windowSize.y / tan(radians(fov) / 2.0);
    return normalize(vec3(xy, -z));
}

float sphereSDF(vec3 point) {
    return length(vec3(0.0, -1.0, 0.0) - point) - SPHERE_RADIUS; 
}

float xzPlaneSDF(vec3 point) {
    return abs(point.y);
}

float sceneSDF(vec3 point) {
    return min(sphereSDF(point), xzPlaneSDF(point));
}

float getShortestDistance(vec3 cameraPos, vec3 dir) {
    float depth = MIN_DIST;
    for (int i = 0; i < MAX_STEPS; i++) {
        float dist = sceneSDF(cameraPos + depth * dir);
        if (dist < EPSILON) {
            return depth;
        }
        depth += dist;
        if (depth >= MAX_DIST) {
            return MAX_DIST;
        }
    }
    return MAX_DIST;
}

void main() {
    vec3 eye = vec3(0, -1.0, 5.0);
    vec3 dir = getRayDirection(vec2(ubo.windowWidth, ubo.windowHeight), gl_FragCoord.xy, 45.0);
    float dist = getShortestDistance(eye, dir);

    if (dist > MAX_DIST - EPSILON) {
        outColor = vec4(0, 0, 0, 1.0);
        return;
    }
    outColor = vec4(dir, 1.0);
}
