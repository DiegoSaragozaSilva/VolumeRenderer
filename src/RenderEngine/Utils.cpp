#include "Utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::vector<uint32_t> Utils::loadShaderCode(std::string shaderPath) {
    // Load shader code from path and convert to uint32_t vector
    std::ifstream shaderStream(shaderPath, std::ios::binary);
    std::vector<unsigned char> shaderBuffer(std::istreambuf_iterator<char>(shaderStream), {});
    std::vector<uint32_t>& shaderCodeBuffer = reinterpret_cast<std::vector<uint32_t>&>(shaderBuffer);
    return shaderCodeBuffer;
}

Mesh* Utils::loadOBJFile(std::string OBJPath) {
    // Load data from file
    tinyobj::attrib_t attribute;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warning, error;
    if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, OBJPath.c_str())) {
        spdlog::error("Failed to load OBJ file: " + warning + error);
        throw 0;
    }

    // Combine all faces vertices and indices 
    uint32_t loadedIndices = 0;
    uint32_t totalIndices = 0;
    for (const auto& shape : shapes)
        totalIndices += shape.mesh.indices.size();

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;

            vertex.position = {
                attribute.vertices[3 * index.vertex_index + 0],
                attribute.vertices[3 * index.vertex_index + 1],
                attribute.vertices[3 * index.vertex_index + 2]
            };

            vertex.normal = {
                attribute.normals[3 * index.normal_index + 0],
                attribute.normals[3 * index.normal_index + 1],
                attribute.normals[3 * index.normal_index + 2]
            };

            vertex.uv = {
                attribute.texcoords[2 * index.texcoord_index + 0],
                1.0f - attribute.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = {1.0f, 1.0f, 1.0f};

            vertices.push_back(vertex);
            indices.push_back(indices.size());

            loadedIndices++;
            spdlog::info("Loading OBJ file " + OBJPath + " [" + std::to_string(((float)loadedIndices / (float)totalIndices) * 100) + "%]");
        }
    }

    // Mesh creation (GPU data is not uploaded by default)
    Mesh* objMesh = new Mesh();
    objMesh->setVertices(vertices);
    objMesh->setIndices(indices);

    return objMesh;
}

ImageData Utils::loadImageFile(std::string imagePath) {
    // Image data struct
    ImageData imageData;
    imageData.depth = 1;

    // Load image with stb_image.h
    uint8_t* _data = stbi_load(imagePath.c_str(), &imageData.width, &imageData.height, &imageData.channels, STBI_rgb_alpha);

    // Check for error loading the image
    if (_data == nullptr) {
        spdlog::error("Failed to load image file: " + imagePath);
        throw 0;
    }

    // Calculate the image data size and store the data into the struct
    uint32_t _dataSize = (imageData.width * imageData.height * (imageData.channels + 1));
    std::cout << imageData.channels << std::endl;
    imageData.data = std::vector<uint8_t>(_data, _data + _dataSize);

    spdlog::info("Image " + imagePath + " successfully loaded.");

    return imageData;
}
