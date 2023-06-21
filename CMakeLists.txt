cmake_minimum_required(VERSION 3.1)
project(VolumeRenderer)

add_compile_options(-O3 -std=c++17)

# Base include directory
include_directories("include" "/usr/lib/stb")

# Required packages
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(spdlog REQUIRED)
find_package(spirv_cross_core REQUIRED)
find_package(DCMTK REQUIRED)
find_package(tinyobjloader REQUIRED)

# Cpp and hpp dependencies
file(GLOB SOURCES src/*/*.hpp src/*/*.cpp)

# Shader compilation to SPIRV
find_package(Vulkan REQUIRED COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

set(SHADER_SOURCE_DIR src/Shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders)

file(
    GLOB SHADERS
    ${SHADER_SOURCE_DIR}/*.vert
    ${SHADER_SOURCE_DIR}/*.frag
    ${SHADER_SOURCE_DIR}/*.geom
)

add_custom_command(
    COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
    OUTPUT ${SHADER_BINARY_DIR}
    COMMENT "Creating ${SHADER_BINARY_DIR}"
)

foreach(source IN LISTS SHADERS)
    get_filename_component(FILENAME ${source} NAME)
    add_custom_command(
        COMMAND
        ${glslc_executable}
        -O0
        -c
        -o ${SHADER_BINARY_DIR}/${FILENAME}.spv
        ${source}
        OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
        DEPENDS ${source} ${SHADER_BINARY_DIR}
        COMMENT "Compiling ${FILENAME} to SPIRV"
    )
    list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

message("DCMTK LIBRARIES" ${DCMTK_LIBRARIES})

# Include directiries
include_directories("include" ${DCMTK_INCLUDE_DIRS})

# Executable
add_executable(Renderer main.cpp ${SOURCES})

# Libray linking
target_link_libraries(Renderer PRIVATE
                      vulkan
                      glfw
                      glm::glm
                      tinyobjloader
                      ${DCMTK_LIBRARIES}
                      spirv-cross-core
                      tiff
                      spdlog::spdlog
                      dcmimgle
                      dcmdata
                      oflog
                      ofstd
                      z)

# Shader custom target
add_custom_target(Shaders ALL DEPENDS ${SPV_SHADERS})
