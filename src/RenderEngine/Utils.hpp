#ifndef _UTILS_H_
#define _UTILS_H_

#include <iostream>
#include <spdlog/spdlog.h>
#include <tiny_obj_loader.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <dcmtk/dcmimgle/dcmimage.h>
#include <tiffio.h>
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

struct TIFFData {
    std::string name;
    uint32_t width, height;
    std::vector<uint8_t> data;
};

struct VolumetricData {
    int width, height, depth;
    glm::vec3 scale;
    std::vector<uint8_t> data;
};

class Utils {
public:
    static std::vector<uint32_t> loadShaderCode(std::string shaderPath);
    static Mesh* loadOBJFile(std::string OBJPath, std::string materialsDir = "");
    static ImageData loadImageFile(std::string imagePath);
    static TIFFData loadTIFFile(std::string TIFFPath);
    static VolumetricData loadVolumetricData(std::string folderPath);
    static std::vector<std::string> listFolderFiles(std::string folderPath);
    static Mesh* getDebugBoxMesh(AABB aabb, glm::vec4 color);
private:
    Utils();
};

#endif
