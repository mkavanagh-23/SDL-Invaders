#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <string>
#include "engine.h"
#include "types.h"
#include "sprite.h"
#include "background.h"

// TODO:
// Set up scoring
  // For each UFO destroyed, playerScore++
  // 40 total ufos... can use either score / 40 to determine round (likely faster)
  // or we can check each row for empty state (likely slower but more logical)
// Set up rounds
  // 3 Rounds
  // Rounds progress when all sprites are destroyed
  // At the start of each round, instantiate a new set of enemy sprites
  // First round - easy (slow), second faster, third randomized value
// Set up lives
  // 3 Lives
  // Lose life when a non-empty row touches a solid tile
  // or do we want to just want to check collision with the player sprite and 'y + height' with the y-axis?
  // Upon lost life, round continues but enemy posititions are reset (destroyed stay destroyed)
// Bind enemy speed to background scroll speed and increment with each round
// The increased scroll speed gives a more stressful feel

/****************************** GLOBAL DATA ***********************************/

// Global Game Variables
int playerScore = 0;
int playerLives = 3;

// Function Prototypes
bool init();

/************************* MAIN GAME FUNCTION ***************************/
int main() {
  
  // Initialize libraries and static data members
  if(!init()) {
    std::cout << "Critical error, terminating program\n";
    return 1;
  }
  
  //Create game objects
  Background background("graphics/bg.bmp");
  Tilemap tilemap("graphics/tilemap.bmp");

  AnimatedSprite sprite("graphics/sprite.bmp", 16, 2, "#000000");
  sprite.setSpeed(5);


  // Tie AlienRow to the scope of a round, generate a new set of aliens
  AlienRow topRow(Rank::first);
  AlienRow upperRow(Rank::second);
  AlienRow lowerRow(Rank::third);
  AlienRow bottomRow(Rank::fourth);

  // BEGIN GAME LOOP
  while(SDL::ProgramIsRunning())
  {
    // Get key press from keyboard and interpret
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    // End game with 'ESC'
    if(keys[SDL_SCANCODE_ESCAPE])
      break;

    // Check for left and right arrow keypresses
    if(keys[SDL_SCANCODE_LEFT]) {
      sprite.setDirection(Direction::left);
      sprite.move();
    }

    if(keys[SDL_SCANCODE_RIGHT]) {
      sprite.setDirection(Direction::right);
      sprite.move();
    }

    sprite.nextFrame();
    sprite.update();
    topRow.update();
    upperRow.update();
    lowerRow.update();
    bottomRow.update();
    background.scroll();
    SDL_RenderClear(SDL::renderer);
    background.draw();
    tilemap.draw();
    sprite.draw();
    topRow.draw();
    upperRow.draw();
    lowerRow.draw();
    bottomRow.draw();
    SDL_RenderPresent(SDL::renderer);
    SDL_Delay(20);
  }
  SDL::CloseShop();
  return 0;
}

bool init() {
  //Initialize SDL
  if(!SDL::Init())
    return false;

  //Initialize static textures
  if(!Alien::init())
    return false;

  return true;
}
