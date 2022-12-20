cmake_minimum_required(VERSION 3.15)

project(03_cube)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_subdirectory(libs/SDL-release-2.26.1)
add_subdirectory(libs/geGL)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm-0.9.9.8)

add_executable(${PROJECT_NAME} src/main.cpp src/cube.hpp src/cube.cpp)

target_link_libraries(${PROJECT_NAME} SDL2-static SDL2::SDL2main geGL::geGL glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

add_custom_target(run ./${PROJECT_NAME})
