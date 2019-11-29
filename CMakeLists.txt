cmake_minimum_required(VERSION 3.3)

project(Homework C)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")

add_definitions(-DSDL_MAIN_HANDLED)

find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)
find_package(SDL2_image REQUIRED)

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_libraries(${GTK3_LIBRARY_DIRS})
    add_definitions(${GTK3_CFLAGS_OTHER})
endif(UNIX)

file(GLOB SRC *.c *.h)

add_executable(${PROJECT_NAME} ${SRC})

set_target_properties(${PROJECT_NAME} PROPERTIES C_STANDARD 99)

target_link_libraries(${PROJECT_NAME} SDL2::Main SDL2::Image SDL2::TTF)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} m)
    target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})
endif(UNIX)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/res
)