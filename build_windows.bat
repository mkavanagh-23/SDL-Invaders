@echo off

echo Creating SDL-Invaders build files...
cmake -B build

echo Linking libraries...
cmake --build build --config Release

echo Build complete! './SDL-Invaders to run!'
