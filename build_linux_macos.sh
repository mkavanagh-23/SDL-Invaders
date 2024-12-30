#!/usr/bin/env sh

printf "Creating SDL-Invaders build files...\n"
cmake -B build

printf "Linking libraries...\n"
cmake --build build --config Release

printf "Build complete! './SDL-Invaders' to run!\n"
