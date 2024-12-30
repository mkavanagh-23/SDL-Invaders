@echo off

echo Creating SDL-Invaders build files...
cmake -B build

echo Linking libraries...
cmake --build build --config Release

echo Build complete! run SDL-Invaders.exe to play!
echo Press any key to exit...
pause
