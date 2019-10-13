cmake_minimum_required(VERSION 3.9)

project(Homework)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_TTF_DIRS})

add_executable(${PROJECT_NAME} main.c Graphics.c Graphics.h debugmalloc.h debugmalloc-impl.h Component.c Component.h Utility.c Utility.h)

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 99)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/res
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)