#include "Voxelizer.hpp"

int Voxelizer::scale = 60;
int Voxelizer::density = 20;

Volume Voxelizer::voxelizeMesh(Mesh* mesh) {
    normalizeMesh(mesh);
    std::vector<Voxel> voxels = getMeshSurfacePoints(mesh);
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
            v.position = point;
            v.normal = normal;

            glm::vec3 color = glm::vec3(0.25f); // glm::linearRand(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
            v.renderData = 0;
            v.renderData += (uint8_t)(color.z * 255.0f);
            v.renderData += (uint8_t)(color.y * 255.0f) << 8;
            v.renderData += (uint8_t)(color.x * 255.0f) << 16;

            voxels.push_back(v);
        }
    }
    return voxels;
}

Mesh* Voxelizer::triangulateVolume(Volume volume) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    uint32_t index = 0;
    float voxelSize = 0.3f;
    for (uint32_t i = 0; i < volume.voxels.size(); i++, index += 8) {
        uint32_t voxelColor = volume.voxels[i].renderData;
        glm::vec3 color = glm::vec3((voxelColor & 0x00110000) >> 16, (voxelColor & 0x00001100) >> 8, voxelColor & 0x0000011);
        Vertex v1 = {
            .position = glm::vec3(-voxelSize, -voxelSize, voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v2 = {
            .position = glm::vec3(voxelSize, -voxelSize, voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v3 = {
            .position = glm::vec3(voxelSize, voxelSize, voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v4 = {
            .position = glm::vec3(-voxelSize, voxelSize, voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v5 = {
            .position = glm::vec3(-voxelSize, -voxelSize, -voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v6 = {
            .position = glm::vec3(voxelSize, -voxelSize, -voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v7 = {
            .position = glm::vec3(voxelSize, voxelSize, -voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
        };

        Vertex v8 = {
            .position = glm::vec3(-voxelSize, voxelSize, -voxelSize) + volume.voxels[i].position,
            .normal = volume.voxels[i].normal,
            .color = color
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

void Voxelizer::removeDuplicatedVoxels(std::vector<Voxel>& voxels) {
    std::cout << "N° of voxels before duplicate removal: " << voxels.size() << std::endl;
    std::sort(voxels.begin(), voxels.end(), [](Voxel& a, Voxel& b) -> bool {
        return a < b;
    });
    voxels.erase(std::unique(voxels.begin(), voxels.end()), voxels.end());
    std::cout << "N° of voxels after duplicate removal: " << voxels.size() << std::endl;
}
