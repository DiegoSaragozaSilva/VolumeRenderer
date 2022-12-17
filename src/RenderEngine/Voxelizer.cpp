#include "Voxelizer.hpp"

int Voxelizer::scale = 60;
int Voxelizer::density = 20;

Volume Voxelizer::voxelizeMesh(Mesh* mesh) {
    normalizeMesh(mesh);
    std::vector<glm::vec3> voxels = getMeshSurfacePoints(mesh);
    removeDuplicatedVoxels(voxels);

    Volume volume = {
        .scale = scale,
        .voxels = voxels
    };

    return volume;
}

void Voxelizer::normalizeMesh(Mesh* mesh) {
    AABB meshBounds = mesh->getBoundingBox();
    glm::vec3 boundSize = {
        std::abs(meshBounds.max.x - meshBounds.min.x),
        std::abs(meshBounds.max.y - meshBounds.min.y),
        std::abs(meshBounds.max.z - meshBounds.min.z)
    };

    float scaleFactor = std::min(scale / boundSize.x, std::min(scale / boundSize.y, scale / boundSize.z));

    glm::mat4 scaleMatrix = glm::mat4(1.0f);
    scaleMatrix = glm::scale(scaleMatrix, glm::vec3(scaleFactor));

    glm::vec3 normalizedCenter = {
        -meshBounds.max.x + (boundSize.z / 2.0f),
        (boundSize.y / 2.0f) - meshBounds.center.y,
        -meshBounds.max.z + (boundSize.z / 2.0f)
    };

    glm::mat4 translationMatrix = glm::mat4(1.0f);
    translationMatrix = glm::translate(translationMatrix, normalizedCenter);

    glm::mat4 finalMatrix = translationMatrix * scaleMatrix;
    mesh->translateByMatrix(finalMatrix);
}

std::vector<glm::vec3> Voxelizer::getMeshSurfacePoints(Mesh* mesh) {
    std::vector<Vertex> vertices = mesh->getVertices();
    std::vector<uint32_t> indices = mesh->getIndices();

    uint32_t numPoints;
    float a, b, c, p, area, lambda, mu;
    glm::vec3 v0, v1, v2, v3, v4, v5, point;
    std::vector<glm::vec3> points;
    for (uint32_t i = 0; i < indices.size(); i += 3) {
        v0 = vertices[indices[i + 0]].position;
        v1 = vertices[indices[i + 1]].position;
        v2 = vertices[indices[i + 2]].position;
        v3 = v0 - v1;
        v4 = v1 - v2;
        v5 = v0 - v2;

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
            points.push_back(point);
        }
    }
    return points;
}

Mesh* Voxelizer::triangulateVolume(Volume volume) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t index = 0;
    float voxelSize = 1.0f;
    for (uint32_t i = 0; i < volume.voxels.size(); i++, index += 8) {
        Vertex v1 = {
            .position = glm::vec3(-voxelSize, -voxelSize, voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v2 = {
            .position = glm::vec3(voxelSize, -voxelSize, voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v3 = {
            .position = glm::vec3(voxelSize, voxelSize, voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v4 = {
            .position = glm::vec3(-voxelSize, voxelSize, voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v5 = {
            .position = glm::vec3(-voxelSize, -voxelSize, -voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v6 = {
            .position = glm::vec3(voxelSize, -voxelSize, -voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v7 = {
            .position = glm::vec3(voxelSize, voxelSize, -voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        Vertex v8 = {
            .position = glm::vec3(-voxelSize, voxelSize, -voxelSize) + volume.voxels[i],
            .color = glm::vec3(0.5f)
        };

        vertices.insert(vertices.end(), {v1, v2, v3, v4, v5, v6, v7, v8});

        indices.insert(indices.end(), {0 + index, 1 + index, 2 + index, 2 + index, 3 + index, 0 + index});
        indices.insert(indices.end(), {1 + index, 5 + index, 6 + index, 6 + index, 2 + index, 1 + index});
        indices.insert(indices.end(), {7 + index, 6 + index, 5 + index, 5 + index, 4 + index, 7 + index});
        indices.insert(indices.end(), {4 + index, 0 + index, 3 + index, 3 + index, 7 + index, 4 + index});
        indices.insert(indices.end(), {4 + index, 5 + index, 1 + index, 1 + index, 0 + index, 4 + index});
        indices.insert(indices.end(), {3 + index, 2 + index, 6 + index, 6 + index, 7 + index, 3 + index});
    }

    Mesh* volumeMesh = new Mesh();
    volumeMesh->setVertices(vertices);
    volumeMesh->setIndices(indices);
    volumeMesh->generateNormals();

    return volumeMesh;
}

void Voxelizer::removeDuplicatedVoxels(std::vector<glm::vec3>& voxels) {
    std::unordered_set<glm::vec3> voxelSet;
    for(uint32_t i = 0; i < voxels.size(); i++ ) voxelSet.insert(voxels[i]);
    voxels.assign(voxelSet.begin(), voxelSet.end());
}
