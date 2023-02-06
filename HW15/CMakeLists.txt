cmake_minimum_required(VERSION 3.13)

project(OPENGL)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BIN_FILE "cube3d")
add_link_options(-lGL -lGLU -lglut )
add_compile_options(-Wall -Wextra -Wpedantic -std=c11)
add_executable(${BIN_FILE} "src/main.c")