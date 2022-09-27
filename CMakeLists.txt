cmake_minimum_required(VERSION 3.1)
project(VolumeRenderer)

add_compile_options(-O3 -Wall -pedantic)

# Build GLFW from include folder
set(GLFW_BUILD_DOCS OFF CACHE BOOL "GLFW lib only")
set(GLFW_INSTALL OFF CACHE BOOL "GLFW lib only")

add_subdirectory("include/glfw")

# Build spdlog from include folder
add_subdirectory("include/spdlog")

# Executable
add_executable(Renderer main.cpp ${Sources})

# Libray linking
target_link_libraries(Renderer PRIVATE spdlog::spdlog glfw vulkan)
