cmake_minimum_required(VERSION 3.14)
project(lab02)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab02 main.cpp glad.c Shader.h Camera.h Object.h Utils.h Mesh.h stb_image.h Skybox.h Car.h Shadow.h Game.h Window.h)
set(SOURCE_FILES main.cpp glad.c)
target_link_libraries(lab02 GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor assimp)