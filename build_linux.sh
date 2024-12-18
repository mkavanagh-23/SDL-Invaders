#!/bin/bash

# CPP compilation script for games programs using the SDL2 Media Library
# Requires installation of SDL2 and additional libraries
# eg. for Arch users: 'sudo pacman -S sdl12-compat sdl2 sdl2_image sdl2_mixer sdl2_ttf sdl_mixer sdl_ttf'

# Usage:
# Copy script to $PATH location, chmod +x, and execute.
#
# If no arguments are provided, processes each .cpp file in the working directory
# creating object files in the /outputs directory. It then calls the linker to generate
# an executable file in the /outputs directory.
#
# If arguments are provided, process each file given, creating objects in the /outputs directory.
# It then calls the linker to generate an executable file in the /outputs directory.

#Variable declaration
usage="Usage: $0 <filename.cpp> <filename2.cpp> ..."
objects=""
output=$(basename "$PWD")
output=$(echo "$output" | tr ' ' '_')
shopt -s nullglob
cpp_files=( *.cpp )

#Create the "outputs" directory
if [[ ! -d "outputs" ]]; then
  mkdir -p "outputs"
fi

#Setup input command string
#If no arguments provided, all *.cpp files are inputs
if [[ $# -eq 0 ]]; then
  #If cpp files present, create input and output command strings
  if [[ ${#cpp_files[@]} -gt 0 ]]; then
    #Compile the object files
    for file in *.cpp; do
      outfile="outputs/${file%.cpp}"
      g++ -std=c++17 -Wall -O2 -DNDEBUG -c "$file" -o "${outfile}.o" &>> outputs/compiler_log.txt
    done
  #Else error and exit the program
  else
	 echo "Error: No '*.cpp' files present in current directory."
	 exit 1
  fi
#If arguments provided, compile object files
else
  for arg in $@; do
    if [[ "arg" != *.cpp ]]; then
      echo "Error: provided argument(s) do not end in '.cpp'."
      echo $usage
      exit 10
    fi
    outfile="outputs/${arg%.cpp}"
    g++ -std=c++17 -Wall -O2 -DNDEBUG -c "$arg" -o "${outfile}.o" &>> outputs/compiler_log.txt
  done
fi

#Link the object files
obj_files=( outputs/*.o )
if [[ ${#obj_files[@]} -gt 0 ]]; then
  for file in outputs/*.o; do
    objects+="$file "
  done
else
  echo "Linking Error: No object files found."
  exit 2
fi

objects="${objects% }"

g++ -std=c++17 $objects -s -lSDL2main -lSDL2 -lSDL3_mixer -o ${output} &> outputs/linker_log.txt

#Check for compilation success or failure
if [[ $? -eq 0 ]]; then
	echo "Program compiled successfully."
	echo "'./$output' to run."
else
	echo "Error building program."
	echo "See 'compiler_log.txt' or 'linker_log.txt' for details."
	exit 3
fi
