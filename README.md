# SDL INVADERS
![sdl invaders](https://github.com/mkavanagh-23/SDL-Invaders/blob/e50ce30b724bbab0c4b9557e076231643073594f/graphics/screenshot.png)

A cross-platform Space Invaders-like game utilizing SDL2. A fun project to expand my game engine knowledge and learn cross-platform build systems. Note that SDL is included as a submodule and compiled as a dynamically linked library.

## Dependencies

- CMake
- [Visual Studio Build Tools](https://aka.ms/vs/17/release/vs_BuildTools.exe) (Windows)
- GCC (Unix).


## Build Instructions:

- Clone the repository recursively
```
git clone --recurse-submodules https://github.com/mkavanagh-23/SDL-Invaders.git
```
- Build with CMake
```
cmake -B build
cmake --build build --config Release
```


## Game Instructions:

The game is similar to "Space Invaders": your ship is under attack and you must defend it from the invading aliens!

The player controls a ship and can move left and right with the respective ARROW KEYS. Bullets can be fired upward towards the invading aliens by pressing the SPACE KEY. The game can be ended at any time by pressing the ESC key.

For each alien destroyed, the player gains a point. If an alien reaches the player or their base, the player loses a life. If the player has no lives remaining, they lose the game. Points and lives can be tracked by checking the game window's title bar.

Gameplay consists of three rounds which advance when the player has destroyed all the aliens on the screen. Each round gets progressively harder as the aliens move faster. Survive all three rounds to win the game, or keep track of your high score to try and beat your friends!
