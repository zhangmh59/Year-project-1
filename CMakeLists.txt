cmake_minimum_required(VERSION 3.15)

project(01_sdl)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_subdirectory(libs/SDL2-2.0.16)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} SDL2-static SDL2::SDL2main)

add_custom_target(run ./${PROJECT_NAME})
