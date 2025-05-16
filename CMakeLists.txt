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

# Output final executable in project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})

add_executable(SDL-Invaders ${SOURCES})

# Include directories
target_include_directories(SDL-Invaders
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

# Find system-installed SDL2
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

# Link the SDL2 libraries
target_link_libraries(SDL-Invaders PRIVATE 
    ${SDL2_LIBRARIES}
)

# Add SDL2 include directories and compiler flags
target_include_directories(SDL-Invaders PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_options(SDL-Invaders PRIVATE ${SDL2_CFLAGS_OTHER})
