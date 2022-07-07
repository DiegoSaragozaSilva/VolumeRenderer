cmake_minimum_required(VERSION 3.10)
project(VolumeRenderer)

add_compile_options(-std=c++17 -O2)

find_package(spdlog REQUIRED)
find_package(glfw3 3.3 REQUIRED)

file(GLOB CppSources src/core/*.cpp)
file(GLOB SpvFiles src/shaders/*.spv)
add_executable(VulkanTest main.cpp ${CppSources})

file(COPY ${SpvFiles} DESTINATION ${CMAKE_BINARY_DIR}/shaders)

target_link_libraries(VulkanTest PRIVATE spdlog::spdlog glfw vulkan)
