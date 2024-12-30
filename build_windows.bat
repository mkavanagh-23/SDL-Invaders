@echo off

echo Creating SDL-Invaders build files...
cmake -B build

echo
echo Building SDL2 and SDL-Invaders...
cmake --build build --config Release

echo Build complete! Run SDL-Invaders.exe to play!
echo
pause
