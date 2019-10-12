cmake_minimum_required(VERSION 3.9)

project(Homework)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} main.c Graphics.c Graphics.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 99)