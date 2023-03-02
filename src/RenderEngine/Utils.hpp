#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <spdlog/spdlog.h>
#include <tinyobjloader/tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include <filesystem>

#include "Mesh.hpp"
#include "Geometry.hpp"

struct ImageData {
    std::string name;
    bool loaded;
    int width, height, depth, channels;
    std::vector<uint8_t> data;
};

class Utils {
public:
    static std::vector<uint32_t> loadShaderCode(std::string shaderPath);
    static Mesh* loadOBJFile(std::string OBJPath, std::string materialsDir = "");
    static ImageData loadImageFile(std::string imagePath);
    static std::vector<std::string> listFolderFiles(std::string folderPath);
    static Mesh* getDebugBoxMesh(AABB aabb, glm::vec3 color);
private:
    Utils();
};

#endif
