cmake_minimum_required(VERSION 3.10)
project(VolumeRenderer)

add_compile_options(-std=c++17 -O2)

find_package(spdlog REQUIRED)
find_package(glfw3 3.3 REQUIRED)

file(GLOB Sources src/*/*.cpp /usr/lib/stb/*.h)
list(REMOVE_ITEM Sources ["src/core/Vulkan.hpp","src/core/Vulkan.cpp"])

file(GLOB SpvFiles src/shaders/*.spv)
file(GLOB Textures src/textures/*)
file(GLOB Volumes src/volumes/*)

add_executable(VulkanTest main.cpp ${Sources})

target_link_libraries(VulkanTest PRIVATE spdlog::spdlog glfw vulkan)
target_include_directories(VulkanTest PRIVATE "/usr/lib/stb/")
target_include_directories(VulkanTest PRIVATE "include")

file(COPY ${SpvFiles} DESTINATION ${CMAKE_BINARY_DIR}/shaders)
file(COPY ${Textures} DESTINATION ${CMAKE_BINARY_DIR}/textures)
file(COPY ${Volumes} DESTINATION ${CMAKE_BINARY_DIR}/volumes)
