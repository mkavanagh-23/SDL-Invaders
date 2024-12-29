cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

project(SDL-Invaders VERSION 1.0.0)

set(SOURCES
    src/background.cpp
    src/engine.cpp
    src/main.cpp
    src/sprite.cpp
    src/types.cpp
)
add_executable(SDL-Invaders ${SOURCES})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

target_include_directories(SDL-Invaders
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(external/SDL2)
add_subdirectory(external/SDL2-Image)

target_link_libraries(SDL-Invaders PRIVATE 
  SDL2
  SDL2_image
)

if (WIN32)
  target_link_libraries(
    SDL-Invaders PRIVATE SDL2main
  )
endif()

add_custom_command(
  TARGET SDL-Invaders POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:SDL2>
    $<TARGET_FILE:SDL2_image>
    $<TARGET_FILE_DIR:SDL-Invaders>
  VERBATIM
)
