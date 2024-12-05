Matthew Kavanagh

SUNY OCC

CSC222-301W  | Fall 2024

![sdl invaders](https://github.com/mkavanagh-23/SDL-Invaders/blob/e50ce30b724bbab0c4b9557e076231643073594f/graphics/screenshot.png)

**Build instruction:**
- Requires the C++14 standard
	-  In Code::Blocks: "Project" ---> ""Build Options..." ---> check the box for "Have g++ follow the C++14 ISO C++ language standard [-std=c++14]"
	-  This should already be configured if opening the .cbp project file, however if issues occur please ensure this is checked.

**Game Instructions:**

The game is similar to "Space Invaders": your ship is under attack and you must defend it from the invading aliens!

The player controls a ship and can move left and right with the respective ARROW KEYS. Bullets can be fired upward towards the invading aliens by pressing the SPACE KEY.

For each alien destroyed, the player gains a point. If an alien reaches the player or their base, the player loses a life. If the player has no lives remaining, they lose the game. Points and lives can be tracked by checking the game window's title bar.

Gameplay consists of three rounds which advance when the player has destroyed all the aliens on the screen. Each round gets progressively harder as the aliens move faster. Survive all three rounds to win the game, or keep track of your high score to try and beat your friends!
