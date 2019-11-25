cmake_minimum_required(VERSION 3.14)
project(GamagoraGL)
set(CMAKE_CXX_STANDARD 17)

set(glfw3_DIR GLFW/lib/cmake/glfw3)
find_package(glfw3 3.3 REQUIRED)

add_subdirectory(assimp-3.3.1)
#set(Assimp_DIR Assimp/lib/cmake/assimp-3.3)
#find_package(assimp REQUIRED)
#if (assimp_FOUND)
#    set(ASSIMP_LIBRARY "assimp")
#    add_library(${ASSIMP_LIBRARY} SHARED IMPORTED)
#    set_target_properties(${ASSIMP_LIBRARY} PROPERTIES IMPORTED_LOCATION "${ASSIMP_LIBRARY_DIRS}/libassimp.dll")
#endif(assimp_FOUND)

include_directories(glad/include)
include_directories(CImg/include)
include_directories(tinyply/include)
include_directories(glm)
include_directories(assimp-3.3.1/include)


configure_file(meshShader.vert ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(meshShader.frag ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(particleShader.vert ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(particleShader.frag ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(logo.stl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(illidan.stl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(arthas.stl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(n64.stl ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(container2.png ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(container2_specular.png ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_library(glad glad/src/glad.c)

add_executable(GamagoraGL main.cpp stl.cpp Shader.cpp Particle.cpp)
target_link_libraries(GamagoraGL glfw glad)
