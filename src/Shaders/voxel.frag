#version 450

#define EPSILON 1e-5
#define INFINITY 3.402823466e+38
#define MAX_STEPS 512

layout (location = 0) out vec4 fragColor;

layout (binding = 0) uniform sampler3D octreeData;

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

// From: https://gist.github.com/companje/29408948f1e8be54dd5733a74ca49bb9
float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

float alphaStep(float alpha) {
  // Tissue
  vec2 tissueDensity = pushConstants.transferAlpha.xy;
  if (alpha >= tissueDensity.x && alpha <= tissueDensity.y) {
    // Left curve
    float leftValue = smoothstep(tissueDensity.x, tissueDensity.y, alpha);
    if (leftValue < 1.0f - EPSILON) {
      float mappedLeftValue = map(leftValue, 0.0f, 1.0f, 0.0f, 0.5f);
      return mappedLeftValue;
    }

    // Right curve
    float rightValue = smoothstep(tissueDensity.y, tissueDensity.x, alpha);
    float mappedRightValue = map(rightValue, 0.0f, 1.0f, 0.0f, 0.5f);
    return mappedRightValue;
  }

  // Bone
  vec2 boneDensity = pushConstants.transferAlpha.zw;
  if (alpha >= boneDensity.x && alpha <= boneDensity.y) {
    // Left curve
    float leftValue = smoothstep(boneDensity.x, boneDensity.y, alpha);
    if (leftValue < 1.0f - EPSILON) return leftValue;

    // Right curve
    float rightCurve = smoothstep(boneDensity.y, boneDensity.x, alpha);
    return rightCurve;
  }

  return 0.0f;
}
 
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
 
bool pointInsideBox(vec3 point, vec3 boxMin, vec3 boxMax) {
    return point.x >= boxMin.x && point.x <= boxMax.x && point.y >= boxMin.y && point.y <= boxMax.y && point.z >= boxMin.z && point.z <= boxMax.z;
}

bool pointInsideSphere(vec3 point, vec3 center, float radius) {
    return distance(point, center) <= radius;
}

vec4 sampleGrid(vec3 index) {
    vec3 trueIndex = vec3(index.x, index.z, 1.0f - index.y) / 2.0f;
    trueIndex += vec3(0.5f, 0.5f, 0.0f);
    trueIndex = clamp(trueIndex, vec3(0.0f), vec3(1.0f));
    
    vec4 color = vec4(texture(octreeData, trueIndex).xyz, 0.0f);

    float meanDensity = (color.x + color.y + color.z) / 3.0f;
    color.w = meanDensity > pushConstants.opacityCutoffMin && meanDensity < pushConstants.opacityCutoffMax ? meanDensity : 0.0f;

    vec4 blendColor = vec4(0.0f);

    // Apply transfer function
    vec4 minColorDensities = vec4(pushConstants.transferRed.x,
                                  pushConstants.transferGreen.x,
                                  pushConstants.transferBlue.x,
                                  pushConstants.transferAlpha.x);

    vec4 maxColorDensities = vec4(pushConstants.transferRed.y,
                                  pushConstants.transferGreen.y,
                                  pushConstants.transferBlue.y,
                                  pushConstants.transferAlpha.w);

    float minDensity = min(minColorDensities.x, min(minColorDensities.y, minColorDensities.z));
    float maxDensity = max(maxColorDensities.x, max(maxColorDensities.y, maxColorDensities.z));

    float mappedDensity = map(color.w, 0.0f, 1.0f, minDensity, maxDensity);

    blendColor.x = smoothstep(minColorDensities.x, maxColorDensities.x, mappedDensity) / maxColorDensities.x;
    blendColor.y = smoothstep(minColorDensities.y, maxColorDensities.y, mappedDensity) / maxColorDensities.y;
    blendColor.z = smoothstep(minColorDensities.z, maxColorDensities.z, mappedDensity) / maxColorDensities.z;
    blendColor.w = color.w;

    blendColor.x = blendColor.x > 0.75f ? 0.0f : blendColor.x;
    blendColor.y = blendColor.y > 0.60f ? 0.0f : blendColor.y;
    blendColor.z = blendColor.z > 0.65f ? 0.0f : blendColor.z;

    return blendColor;
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
        vec3 accumulatedColor = vec3(0.0f);
        float accumulatedAlpha = 0.0f;
        for (int i = 0; i < MAX_STEPS; i++) {
            if (accumulatedAlpha > 0.95f || equal(voxel, endVoxel) == bvec3(true)) break;

            vec3 voxelIndex = voxel * voxelSize;
            vec4 voxelColor;

            bool planeComparison = pushConstants.planeCutoff.w == -1.0f ? greaterThanEqual(voxel, cuttingPlane) == bvec3(false) : greaterThanEqual(voxel, cuttingPlane) != bvec3(false);
            if (planeComparison)
              voxelColor = sampleGrid(voxelIndex);
            else
              voxelColor = vec4(0.0f);

            accumulatedColor += (1.0f - accumulatedAlpha) * (voxelColor.w * voxelColor.xyz);
            accumulatedAlpha += (1.0f - accumulatedAlpha) * voxelColor.w;

            mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
            sideDist += vec3(mask) * deltaDist;
            voxel += vec3(mask) * rayStep;
        }

        accumulatedColor += (1.0f - accumulatedAlpha) * (1.0f * skyColor(ray));
        return accumulatedColor;
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
