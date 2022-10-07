cmake_minimum_required(VERSION 3.1)
project(VolumeRenderer)

add_compile_options(-O3)

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

# Force Vulkan packaged be required
find_package(Vulkan REQUIRED)

# Cpp and hpp dependencies
file(GLOB SOURCES src/*/*.hpp src/*/*.cpp)

# Executable
add_executable(Renderer main.cpp ${SOURCES})

# Include directory
include_directories("include")

# Libray linking
target_link_libraries(Renderer PRIVATE spdlog::spdlog glfw glm vulkan VulkanMemoryAllocator)
