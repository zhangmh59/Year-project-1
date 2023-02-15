cmake_minimum_required(VERSION 3.15)

project(Skyboxes)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_subdirectory(Libraries/glfw-3.3.8)

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm)

add_executable(${PROJECT_NAME} 
  src/Camera.cpp
  src/Camera.h
  src/EBO.cpp
  src/EBO.h
  src/glad.c
  src/Main.cpp
  src/Mesh.cpp
  src/Mesh.h
  src/Model.cpp
  src/Model.h
  src/shaderClass.cpp
  src/shaderClass.h
  src/stb.cpp
  src/Texture.cpp
  src/Texture.h
  src/VAO.cpp
  src/VAO.h
  src/VBO.cpp
  src/VBO.h
  )

target_link_libraries(${PROJECT_NAME} glm glfw)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_target(run ./${PROJECT_NAME})


