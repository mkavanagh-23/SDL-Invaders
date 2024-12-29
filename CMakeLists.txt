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
add_executable(SDL-Invaders ${SOURCES})

# Include directories
target_include_directories(SDL-Invaders
  PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Add third-party libraries
add_subdirectory(external/SDL2)

# Link the external libraries
target_link_libraries(SDL-Invaders PRIVATE 
  SDL2
)

# Set rpath for SDL2 libraries on macOS
set(CMAKE_INSTALL_RPATH "@executable_path")
# For SDL2_image (or other libraries), you might also want to add it
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Link SDL2main if building for Windows
if(WIN32)
  #set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
  target_link_libraries(
    SDL-Invaders PRIVATE SDL2main
  )
endif()

# Copy runtime DLLs to the output directory
add_custom_command(
  TARGET SDL-Invaders POST_BUILD COMMAND
  ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:SDL2>"
    "$<TARGET_FILE_DIR:SDL-Invaders>"
  VERBATIM
)
