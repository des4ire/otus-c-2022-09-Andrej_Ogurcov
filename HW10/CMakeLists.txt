cmake_minimum_required(VERSION 3.13)

project(CRC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(BIN_FILE "crc-32")

add_compile_options(-Wall -Wextra -Wpedantic)
add_executable(${BIN_FILE} "src/main.c")
