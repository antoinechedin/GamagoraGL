cmake_minimum_required(VERSION 3.16)
project(GamagoraGL)
set(CMAKE_CXX_STANDARD 17)

set(glfw3_DIR C:/Program\ Files\ \(x86\)/GLFW/lib/cmake/glfw3)
find_package(glfw3 3.3 REQUIRED)

include_directories(glad/include)
include_directories(CImg/include)
include_directories(tinyply/include)
include_directories(glm)

add_library(glad glad/src/glad.c)

add_executable(GamagoraGL main.cpp stl.cpp)
target_link_libraries(GamagoraGL glfw glad)
