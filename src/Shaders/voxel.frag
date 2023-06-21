#version 450

#define EPSILON 1e-5
#define INFINITY 3.402823466e+38
#define MAX_STEPS 512
#define ALPHA_THRESHOLD 0.99

layout (location = 0) out vec4 fragColor;

layout (set = 0, binding = 0) uniform sampler3D octreeData;
layout (set = 0, binding = 1) uniform sampler1D transferFunction;

layout (std430, push_constant) uniform PushConstants {
    mat4 cameraView;
    mat4 cameraProjection;
    vec4 transferAlpha;
    vec4 planeCutoff;
    vec2 windowDimensions;
    vec2 transferRed;
    vec2 transferGreen;
    vec2 transferBlue;
    vec3 cameraPos;
    float octreeDepth;
    vec3 octreeMin;
    float opacityCutoffMin;
    vec3 octreeMax;
    float opacityCutoffMax;
} pushConstants;

struct Ray {
    vec3 origin;
    vec3 direction;
};
 
bool hitBox(Ray ray, vec3 boxMin, vec3 boxMax, inout float tmin, inout float tmax) {
    tmin = 0.0f;
    tmax = INFINITY;
    
    for (int d = 0; d < 3; d++) {
        float t1 = (boxMin[d] - ray.origin[d]) / ray.direction[d];
        float t2 = (boxMax[d] - ray.origin[d]) / ray.direction[d];
        
        tmin = max(tmin, min(t1, t2));
        tmax = min(tmax, max(t1, t2));
    }
    
    return tmin < tmax;
}

float sampleGrid(vec3 index) {
    vec3 trueIndex = vec3(index.x, index.z, 1.0f - index.y) / 2.0f;
    trueIndex += vec3(0.5f, 0.5f, 0.0f);
    trueIndex = clamp(trueIndex, vec3(-1.0f), vec3(1.0f));
    
    vec3 color = texture(octreeData, trueIndex).xyz;
    float meanDensity = (color.x + color.y + color.z) / 3.0f;
    meanDensity = meanDensity > pushConstants.opacityCutoffMin && meanDensity < pushConstants.opacityCutoffMax ? meanDensity : 0.0f;
    return meanDensity;
}
 
vec3 skyColor(Ray ray) {
  return vec3(0.0f);
}
 
vec3 rayColor(Ray ray) {
    vec3 voxelSize = 1.0f / (pushConstants.octreeMax.xyz - pushConstants.octreeMin.xyz);
    vec3 _gridMin = vec3(-1.0f) / voxelSize;
    vec3 _gridMax = vec3(1.0f) / voxelSize;
    
    Ray gridRay;
    gridRay.origin = ray.origin / voxelSize;
    gridRay.direction = ray.direction;

    vec3 cuttingPlane = _gridMin + (_gridMax - _gridMin) * (1.0f - pushConstants.planeCutoff.xyz);

    float tmin, tmax;
    if (hitBox(gridRay, _gridMin, _gridMax, tmin, tmax)) {
        vec3 ro = (gridRay.origin + gridRay.direction * tmin);
        vec3 re = (gridRay.origin + gridRay.direction * tmax);
        vec3 voxel = floor(ro);
        vec3 endVoxel = floor(re);

        vec3 deltaDist = abs(1.0f / gridRay.direction);
        vec3 rayStep = sign(gridRay.direction);
        vec3 sideDist = (sign(gridRay.direction) * (vec3(voxel) - ro) +
                        (sign(gridRay.direction) * 0.5f) + 0.5f) * deltaDist;

        bvec3 mask;
        vec4 accumulatedColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        for (int i = 0; i < MAX_STEPS && accumulatedColor.w < ALPHA_THRESHOLD && equal(voxel, endVoxel) != bvec3(true); i++) { 
            vec3 voxelIndex = voxel * voxelSize;

            float voxelSample = 0.0f;
            bool planeComparison = pushConstants.planeCutoff.w == -1.0f ? greaterThanEqual(voxel, cuttingPlane) == bvec3(false) : greaterThanEqual(voxel, cuttingPlane) != bvec3(false);
            if (planeComparison)
               voxelSample = sampleGrid(voxelIndex);

            vec4 transferFunctionData = texture(transferFunction, voxelSample);
            if (voxelSample > 0.0f) {
                // voxelSample = 1.0f - pow(1.0f - accumulatedColor.w, length(rayStep) * 0.5f * 200.0f);

                accumulatedColor.xyz += (1.0f - accumulatedColor.w) * transferFunctionData.xyz * voxelSample;
                accumulatedColor.w += (1.0f - accumulatedColor.w) * voxelSample;
            }

            mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
            sideDist += vec3(mask) * deltaDist;
            voxel += vec3(mask) * rayStep;
        }

        return accumulatedColor.xyz;
    }

    return skyColor(ray);
}

void main() {   
    vec2 coord = vec2(gl_FragCoord.x / pushConstants.windowDimensions.x, gl_FragCoord.y / pushConstants.windowDimensions.y);
    coord = coord * 2.0f - 1.0f;

    vec4 target = inverse(pushConstants.cameraProjection) * vec4(coord.x, coord.y, 1.0f, 1.0f);

    Ray ray;
    ray.origin = pushConstants.cameraPos.xyz;
    ray.direction = vec3(inverse(pushConstants.cameraView) * vec4(normalize(vec3(target) / target.w), 0.0f));

    fragColor = vec4(rayColor(ray), 1.0f);    
}
