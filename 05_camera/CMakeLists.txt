cmake_minimum_required(VERSION 3.15)

project(05_camera)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_subdirectory(libs/SDL-release-2.26.1)
add_subdirectory(libs/geGL)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm-0.9.9.8)

set(LIBS_SOURCES
  libs/tiny_gltf/tiny_gltf.h
  libs/stb_image/stb_image.h
  libs/stb_image/stb_image_write.h
  libs/json/json.hpp
  libs/impl.cpp
  )

add_executable(${PROJECT_NAME} 
  src/main.cpp 
  ${LIBS_SOURCES}
  )

target_link_libraries(${PROJECT_NAME} SDL2-static SDL2::SDL2main geGL::geGL glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/json)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm-0.9.9.8)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(run ./${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
