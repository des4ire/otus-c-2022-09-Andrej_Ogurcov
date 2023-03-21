cmake_minimum_required(VERSION 3.13)

project(DB)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BIN_FILE "sqlite")

add_compile_options(-Wall -Wextra -Wpedantic)
add_link_options(-lsqlite3)
add_executable(${BIN_FILE} "src/main.c")