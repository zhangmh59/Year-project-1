cmake_minimum_required(VERSION 3.15)

project(08-music)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_subdirectory(libs/glfw-3.3.8)
add_subdirectory(libs/SDL-release-2.26.1)
add_subdirectory(libs/geGL)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm)


set(LIBS_SOURCES
  libs/tiny_gltf/tiny_gltf.h
  libs/stb_image/stb_image.h
  libs/stb_image/stb_image_write.h
  libs/json/json.hpp
  libs/impl.cpp
  )


add_executable(${PROJECT_NAME} 
  src/audio.h
  src/audio.c
  src/Camera.cpp
  src/Camera.h
  src/EBO.cpp
  src/EBO.h
  src/glad.c
  src/Main.cpp
  #src/Mesh.cpp
  #src/Mesh.h
  #src/Model.cpp
  #src/Model.h
  src/shaderClass.cpp
  src/shaderClass.h
  src/stb.cpp
  src/Texture.cpp
  src/Texture.h
  src/VAO.cpp
  src/VAO.h
  src/VBO.cpp
  src/VBO.h
  ${LIBS_SOURCES}
  )

target_link_libraries(${PROJECT_NAME} glm glfw SDL2-static SDL2::SDL2main geGL::geGL)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/json)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL-release-2.26.1)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(run ./${PROJECT_NAME})


