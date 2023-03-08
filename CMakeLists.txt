cmake_minimum_required(VERSION 3.1)
project(VolumeRenderer)

add_compile_options(-O3)

# Base include directory
include_directories("include")

# Build GLFW from include folder
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_subdirectory("include/glfw")

# Build spdlog from include folder
add_subdirectory("include/spdlog")

# Build VulkanMemoryAllocator from include folder
add_subdirectory("include/VulkanMemoryAllocator")

# Build glm from include folder
add_subdirectory("include/glm")

# Build SPIRV-Cross from include folder
add_subdirectory("include/SPIRV-Cross")

# Build tinyobjloader from include folder
add_subdirectory("include/tinyobjloader")

# Set up Imgui
set(IMGUI_DIR include/imgui)
add_library(IMGUI STATIC)

target_sources(
    IMGUI
    PRIVATE
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp

    PRIVATE
    ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
)

target_include_directories(
    IMGUI
    PUBLIC ${IMGUI_DIR}
    PUBLIC ${IMGUI_DIR}/backends
    PUBLIC ${GLFW_DIR}/include
)

target_link_libraries(IMGUI PUBLIC glfw)

# Force Vulkan packaged be required
find_package(Vulkan REQUIRED)

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

# Executable
add_executable(Renderer main.cpp ${SOURCES})

# Include directory
include_directories("include")

# Libray linking
target_link_libraries(Renderer PRIVATE spdlog::spdlog glfw glm vulkan spirv-cross-core tinyobjloader IMGUI)

# Shader custom target
add_custom_target(Shaders ALL DEPENDS ${SPV_SHADERS})
