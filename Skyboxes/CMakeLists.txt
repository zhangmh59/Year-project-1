cmake_minimum_required(VERSION 3.15)

project(Skyboxes)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1 ) 

add_library(glm INTERFACE)
target_include_directories(glm INTERFACE libs/glm)

set(LIBS_SOURCES
  libs/stb/stb_image.h
  libs/json/json.h
  )

add_executable(${PROJECT_NAME} 
  src/main.cpp 
  ${LIBS_SOURCES}
  )

target_link_libraries(${PROJECT_NAME} SDL2-static SDL2::SDL2main geGL::geGL glm)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/json)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm-0.9.9.8)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(run ./${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKE_ROOT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
