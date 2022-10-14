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

Mesh* Utils::loadOBJFile(std::string OBJPath, std::string materialsDir) {
    // Load data from file
    tinyobj::attrib_t attribute;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warning, error;
    if (!tinyobj::LoadObj(&attribute, &shapes, &materials, &warning, &error, OBJPath.c_str(), materialsDir == "" ? NULL : materialsDir.c_str())) {
        spdlog::error("Failed to load OBJ file: " + warning + error);
        throw 0;
    }

    // Parse all the materials
    std::vector<Material> objMaterials(materials.size());
    for (size_t i = 0; i < materials.size(); i++) {
        Material _material;
        _material.ambientColor = { materials[i].ambient[0], materials[i].ambient[1], materials[i].ambient[2] };
        _material.diffuseColor = { materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2] };
        _material.specularColor = { materials[i].specular[0], materials[i].specular[1], materials[i].specular[2] };
        _material.transmittanceColor = { materials[i].transmittance[0], materials[i].transmittance[1], materials[i].transmittance[2] };
        _material.emissionColor = { materials[i].emission[0], materials[i].emission[1], materials[i].emission[2] };
        _material.specularExponent = materials[i].shininess;
        _material.indexOfRefraction = materials[i].ior;
        _material.transparency = materials[i].dissolve;
        _material.illuminationModel = materials[i].illum;
        _material.ambientTextureMap = materials[i].ambient_texname;
        _material.diffuseTextureMap = materials[i].diffuse_texname;
        _material.specularColorMap = materials[i].specular_texname;
        _material.specularHighlightTextureMap = materials[i].specular_highlight_texname;
        _material.alphaTextureMap = materials[i].alpha_texname;
        _material.bumpTextureMap = materials[i].bump_texname;
        _material.displacementTextureMap = materials[i].displacement_texname;
        _material.reflectionTextureMap = materials[i].reflection_texname;
        objMaterials[i] = _material;
    }

    // Combine all faces vertices and indices 
    uint32_t i = 0;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices; 
    std::vector<Material> _materials;
    for (const auto& shape : shapes) {
        _materials.push_back(objMaterials[shape.mesh.material_ids[0]]);
        _materials[_materials.size() - 1].indexCount = shape.mesh.indices.size();
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
        }
        i++;
    }

    std::cout << _materials.size() << std::endl;

    // Mesh creation (GPU data is not uploaded by default)
    Mesh* objMesh = new Mesh();
    objMesh->setVertices(vertices);
    objMesh->setIndices(indices);
    objMesh->setMaterials(_materials);

    spdlog::info("OBJ file " + OBJPath + " successfully loaded.");

    return objMesh;
}

ImageData Utils::loadImageFile(std::string imagePath) {
    // Image data struct
    ImageData imageData;
    imageData.name = imagePath;
    imageData.loaded = true;
    imageData.depth = 1;

    // Load image with stb_image.h
    uint8_t* _data = stbi_load(imagePath.c_str(), &imageData.width, &imageData.height, &imageData.channels, STBI_rgb_alpha);

    // Check for error loading the image
    if (_data == nullptr) {
        spdlog::warn("Failed to load image file: " + imagePath);
        imageData.loaded = false;
        return imageData;
    }

    // Calculate the image data size and store the data into the struct
    uint32_t _dataSize = (imageData.width * imageData.height * (imageData.channels + 1));
    imageData.data = std::vector<uint8_t>(_data, _data + _dataSize);

    spdlog::info("Image " + imagePath + " successfully loaded.");

    return imageData;
}
