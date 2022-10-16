#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <spdlog/spdlog.h>
#include <tinyobjloader/tiny_obj_loader.h>
#include <string>
#include <vector>
#include <fstream>
#include <iterator>
#include "Mesh.hpp"

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
    static glm::vec3 calculateFaceNormal(std::vector<glm::vec3> face);
private:
    Utils();
};

#endif
