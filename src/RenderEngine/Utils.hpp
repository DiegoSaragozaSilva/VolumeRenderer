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
    int width, height, depth, channels;
    std::vector<uint8_t> data;
};

class Utils {
public:
    static std::vector<uint32_t> loadShaderCode(std::string shaderPath);
    static Mesh* loadOBJFile(std::string OBJPath);
    static ImageData loadImageFile(std::string imagePath);
private:
    Utils();
};

#endif
