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
        
        if (objMaterials.size() > 0) {
            _materials.push_back(objMaterials[shape.mesh.material_ids[0]]);
            _materials[_materials.size() - 1].indexCount = shape.mesh.indices.size();
        }
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;
            
            if (index.vertex_index != -1) {
                vertex.position = {
                    attribute.vertices[3 * index.vertex_index + 0],
                    attribute.vertices[3 * index.vertex_index + 1],
                    attribute.vertices[3 * index.vertex_index + 2]
                };
            }

            if (index.normal_index != -1) {
                vertex.normal = {
                    attribute.normals[3 * index.normal_index + 0],
                    attribute.normals[3 * index.normal_index + 1],
                    attribute.normals[3 * index.normal_index + 2]
                };
            }

            if (index.texcoord_index != -1) {
                vertex.uv = {
                    attribute.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attribute.texcoords[2 * index.texcoord_index + 1]
                };
            }

            vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
        i++;
    }

    // Mesh creation (GPU data is not uploaded by default)
    Mesh* objMesh = new Mesh();
    objMesh->setVertices(vertices);
    objMesh->setIndices(indices);
    objMesh->setMaterials(_materials);
    
    // If no normals are passed, generate them
    if (attribute.normals.size() == 0)
        objMesh->generateNormals();

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

std::vector<std::string> Utils::listFolderFiles(std::string folderPath) {
    // List files in folder and return a vector with them
    std::vector<std::string> files;
    for (const auto& file : std::filesystem::directory_iterator(folderPath))
        files.push_back(file.path());
    return files;
}

Mesh* Utils::getDebugBoxMesh(AABB aabb, glm::vec4 color) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices; 
    std::vector<Material> _materials;

    glm::vec3 aabbSize = aabb.max - aabb.min;

    Vertex v1 = {
        .position = aabb.min,
        .normal = glm::vec3(0.0f),
        .color = color
    };

    Vertex v2 = {
        .position = aabb.min + glm::vec3(aabbSize.x, 0.0f, 0.0f),
        .normal = glm::vec3(0.0f),
        .color = color
    };

    Vertex v3 = {
        .position = aabb.min + glm::vec3(aabbSize.x, aabbSize.y, 0.0f),
        .normal = glm::vec3(0.0f),
        .color = color
    };

    Vertex v4 = {
        .position = aabb.min + glm::vec3(0.0f, aabbSize.y, 0.0f),
        .normal = glm::vec3(0.0f),
        .color = color
    };

    Vertex v5 = {
        .position = aabb.min + glm::vec3(0.0f, 0.0f, aabbSize.z),
        .normal = aabb.min,
        .color = color
    };

    Vertex v6 = {
        .position = aabb.min + glm::vec3(aabbSize.x, 0.0f, aabbSize.z),
        .normal = glm::vec3(0.0f),
        .color = color
    };

    Vertex v7 = {
        .position = aabb.min + glm::vec3(aabbSize.x, aabbSize.y, aabbSize.z),
        .normal = glm::vec3(0.0f),
        .color = color
    };

    Vertex v8 = {
        .position = aabb.min + glm::vec3(0.0f, aabbSize.y, aabbSize.z),
        .normal = glm::vec3(0.0f),
        .color = color
    };

    vertices.insert(vertices.end(), {v1, v2, v3, v4, v5, v6, v7, v8});

    indices.insert(indices.end(), {0, 1, 1, 2, 2, 3, 3, 0});
    indices.insert(indices.end(), {4, 5, 5, 6, 6, 7, 7, 4});
    indices.insert(indices.end(), {0, 4, 1, 5, 2, 6, 3, 7});

    Mesh* debugBoxMesh = new Mesh();
    debugBoxMesh->setVertices(vertices);
    debugBoxMesh->setIndices(indices);
    return debugBoxMesh;
}

TIFFData Utils::loadTIFFile(std::string TIFFPath) {
    TIFF* file = TIFFOpen(TIFFPath.c_str(), "r");
    if (file == NULL) {
         spdlog::warn("Unable to read TIFF file " + TIFFPath);
         return {};
    }

    TIFFData data;
    data.name = TIFFPath;
    TIFFGetField(file, TIFFTAG_IMAGEWIDTH, &data.width);
    TIFFGetField(file, TIFFTAG_IMAGELENGTH, &data.height);

    uint32_t* rasterData = (uint32_t*)_TIFFmalloc((data.width * data.height) * sizeof(uint32_t));
    if (rasterData != NULL) {
        if (TIFFReadRGBAImage(file, data.width, data.height, rasterData, 0)) {
            
            for (uint32_t y = 0; y < data.height; y++)
                for (uint32_t x = 0; x < data.width; x++) {
                    uint32_t pixel = rasterData[y * data.width + x];
                    data.data.push_back(TIFFGetR(pixel));
                    data.data.push_back(TIFFGetG(pixel));
                    data.data.push_back(TIFFGetB(pixel));
                }
        }
        _TIFFfree(rasterData);
    }
    TIFFClose(file);

    return data;
}

VolumetricData Utils::loadVolumetricData(std::string folderPath) {
    std::vector<std::string> files = listFolderFiles(folderPath);
    std::sort(files.begin(), files.end());

    VolumetricData volumetricData;
    volumetricData.width = 0;
    volumetricData.height = 0;
    volumetricData.depth = files.size();

    if (files.size() == 0) return volumetricData;

    if (files[0].find(".tif") != std::string::npos) {
      for (auto file : files) {
          TIFFData data = loadTIFFile(file);
          volumetricData.data.insert(volumetricData.data.end(), data.data.begin(), data.data.end());

          if (volumetricData.width == 0 && volumetricData.height == 0) {
              volumetricData.width = data.width;
              volumetricData.height = data.height;
          }
      }
    }
    else if (files[0].find(".dcm") != std::string::npos) {
        for (auto file : files) {
            DicomImage* dcmImage = new DicomImage(file.c_str());
            if (dcmImage == NULL) {
                spdlog::error("Error while loading DICOM file " + file);
                throw 0;
            }
            
            if (volumetricData.width == 0 && volumetricData.height == 0) {
                volumetricData.width = dcmImage->getWidth();
                volumetricData.height = dcmImage->getHeight();
                volumetricData.scale = glm::vec3(1.0f / volumetricData.width, 1.0f / volumetricData.height, 1.0f / volumetricData.depth);
            }
            
            dcmImage->setMinMaxWindow();

            uint8_t* dcmPixels = (uint8_t*)dcmImage->getOutputData(8);
            if (dcmPixels == NULL) {
                spdlog::error("Error while getting pixel data from DICOM file " + file);
                throw 0;
            }
            std::vector<uint8_t> dcmData = std::vector<uint8_t>(dcmPixels, dcmPixels + dcmImage->getOutputDataSize(8));
            volumetricData.data.insert(volumetricData.data.end(), dcmData.begin(), dcmData.end());
        }
    }

    return volumetricData;
}
