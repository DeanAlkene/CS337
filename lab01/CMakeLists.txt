cmake_minimum_required(VERSION 3.14)
project(lab01)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab01 main.cpp glad.c Shader.h Camera.h Object.h Utils.h)
set(SOURCE_FILES main.cpp glad.c)
target_link_libraries(lab01 GL GLU glfw3 X11 Xxf86vm Xrandr pthread Xi dl Xinerama Xcursor)