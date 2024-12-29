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

target_include_directories(SDL-Invaders
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include/
