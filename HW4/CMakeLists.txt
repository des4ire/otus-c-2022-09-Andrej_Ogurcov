cmake_minimum_required(VERSION 3.5)

project(weather LANGUAGES C)


add_executable(weather src/main.c)
find_package(json-c REQUIRED )
target_link_libraries(${PROJECT_NAME} PRIVATE json-c::json-c)
find_package( CURL REQUIRED )
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)		

add_compile_options( -Wall -Wextra -Wpedantic -std=c11) 

