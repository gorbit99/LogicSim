cmake_minimum_required(VERSION 3.3)

project(Homework)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)
find_package(SDL2_image REQUIRED)

add_executable(${PROJECT_NAME} main.c Graphics.c Graphics.h debugmalloc.h debugmalloc-impl.h Component.c Component.h
                                Utility.c Utility.h Window.c Window.h Input.c Input.h Node.c Node.h Parser.c Parser.h
                                NodeVector.c NodeVector.h Camera.c Camera.h GUIGraphics.c GUIGraphics.h NodeEvent.c
                                NodeEvent.h Wire.c Wire.h)

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 99)

target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image SDL2::TTF)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)