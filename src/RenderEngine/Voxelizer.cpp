#include "Voxelizer.hpp"

int Voxelizer::scale = 60;
int Voxelizer::density = 20;

Volume Voxelizer::voxelizeMesh(Mesh* mesh) {
    normalizeMesh(mesh);
    std::vector<Voxel> voxels = getMeshSurfacePoints(mesh);
    removeDuplicatedVoxels(voxels);

    Volume volume = {
        .scale = glm::vec3(scale),
        .voxels = voxels
    };

    return volume;
}

void Voxelizer::normalizeMesh(Mesh* mesh) {
    AABB meshBounds = mesh->getBoundingBox();
    glm::vec3 boundSize = {
        std::abs(meshBounds.min.x - meshBounds.max.x),
        std::abs(meshBounds.min.y - meshBounds.max.y),
        std::abs(meshBounds.min.z - meshBounds.max.z)
    };

    float scaleFactor = std::min(scale / boundSize.x, std::min(scale / boundSize.y, scale / boundSize.z));

    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scaleFactor));

    glm::vec3 normalizedCenter = {
        -meshBounds.max.x + (boundSize.x / 2.0f),
        (boundSize.y / 2.0f) - meshBounds.max.y,
        -meshBounds.center.z + (boundSize.z / 2.0f)
    };

    glm::mat4 translationMatrix = glm::mat4(1.0f);
    translationMatrix = glm::translate(translationMatrix, normalizedCenter);

    glm::mat4 finalMatrix = scaleMatrix * translationMatrix;
    mesh->translateByMatrix(finalMatrix);
}

std::vector<Voxel> Voxelizer::getMeshSurfacePoints(Mesh* mesh) {
    std::vector<Vertex> vertices = mesh->getVertices();
    std::vector<uint32_t> indices = mesh->getIndices();

    uint32_t numPoints;
    float a, b, c, p, area, lambda, mu;
    glm::vec3 v0, v1, v2, v3, v4, v5, point, normal;
    std::vector<Voxel> voxels;
    for (uint32_t i = 0; i < indices.size(); i += 3) {
        v0 = vertices[indices[i + 0]].position;
        v1 = vertices[indices[i + 1]].position;
        v2 = vertices[indices[i + 2]].position;
        v3 = v0 - v1;
        v4 = v1 - v2;
        v5 = v0 - v2;
        normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

        a = glm::length(v3);
        b = glm::length(v4);
        c = glm::length(v5);
        p = (a + b + c) / 2.0f;
        area = std::sqrt(p * (p - a) * (p - b) * (p - c));
        numPoints = std::round(density * area);
        for (uint32_t j = 0; j < numPoints; j++) {
            lambda = glm::linearRand(0.0f, 1.0f);
            mu = glm::linearRand(0.0f, 1.0f);
            point = (v0 + (lambda * v3)) + (v4 * (lambda * mu));
            
            Voxel v;
            v.color = glm::vec4(glm::linearRand(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f)), 1.0f);
            v.position = point;
            v.normal = normal;
            voxels.push_back(v);
        }
    }
    return voxels;
}

void Voxelizer::removeDuplicatedVoxels(std::vector<Voxel>& voxels) {
    std::sort(voxels.begin(), voxels.end(), [](Voxel& a, Voxel& b) -> bool {
        return a < b;
    });
    voxels.erase(std::unique(voxels.begin(), voxels.end()), voxels.end());
}

Volume Voxelizer::voxelizeVolumetricData(VolumetricData data) { 
    Volume volume;
    volume.scale = data.scale;

    for (uint32_t y = 0; y < data.height / 3; y++)
        for (uint32_t z = 0; z < data.depth / 3; z++)
            for (uint32_t x = 0; x < data.width / 3; x++) {
                uint32_t index = x + data.width * (y + data.height * z);
                glm::vec4 pixelData = glm::vec4(
                  data.data[index * 3 + 0],
                  data.data[index * 3 + 1],
                  data.data[index * 3 + 2],
                  255.0f
                ) / 255.0f;

                Voxel voxel;
                voxel.normal = glm::vec3(0.0f);
                voxel.position = glm::vec3(x, z, y);
                voxel.color = pixelData;
                volume.voxels.push_back(voxel);
            }
    return volume;
}
