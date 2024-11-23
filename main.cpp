#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
#include "engine.h"
#include "types.h"
#include "sprite.h"
#include "background.h"
#include "settings.h"

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
bool playGame = false;  // Menu state variable

// Declare Global game objects
Background* background;
Tilemap* tilemap;
AnimatedSprite* logo;
AnimatedSprite* start;
AnimatedSprite* player;
AlienRow* topRow;
AlienRow* upperRow;
AlienRow* lowerRow;
AlienRow* bottomRow;

// Function Prototypes
// Game state functions
bool init();
void end();
void createObjects();
void destroyObjects();
// Gameplay functions
void displayMenu();
void startGame();
void updateObjects();
void drawObjects();

/************************* MAIN GAME FUNCTION ***************************/
int main() {
  
  // Initialize libraries and static data members
  if(!init()) {
    std::cout << "Critical error, terminating program\n";
    return 1;
  }

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
      player->setDirection(Direction::left);
      player->move();
    }

    if(keys[SDL_SCANCODE_RIGHT]) {
      player->setDirection(Direction::right);
      player->move();
    }

    // Display a menu until the player quits or selects 'START'
    if(!playGame) {
      if(keys[SDL_SCANCODE_SPACE]) {
        playGame = true;
        background->scroll();
        SDL_RenderClear(SDL::renderer);
        background->draw();
        logo->draw();
        SDL_RenderPresent(SDL::renderer);
        SDL_Delay(1000);
        continue;
      }
      player->nextFrame();
      player->update();
      start->nextFrame();
      background->scroll();
      SDL_RenderClear(SDL::renderer);
      background->draw();
      logo->draw();
      start->draw();
      player->draw();
      SDL_RenderPresent(SDL::renderer);
      SDL_Delay(20);
    }   // END DISPLAY MENU
   // Play the game 
    else {
      player->nextFrame();
      player->update();
      topRow->update();
      upperRow->update();
      lowerRow->update();
      bottomRow->update();
      background->scroll();
      SDL_RenderClear(SDL::renderer);
      background->draw();
      tilemap->draw();
      player->draw();
      topRow->draw();
      upperRow->draw();
      lowerRow->draw();
      bottomRow->draw();
      SDL_RenderPresent(SDL::renderer);
      SDL_Delay(20);
    }
  }
  end();
  return 0;
}

bool init() {
  //Initialize SDL
  if(!SDL::Init())
    return false;

  //Initialize static textures
  if(!Alien::init())
    return false;

  //Create game objects
  createObjects();

  return true;
}

void end() {
  destroyObjects();
  SDL::CloseShop();
}

void createObjects() {
  background = new Background("graphics/bg.bmp");
  tilemap = new Tilemap("graphics/tilemap.bmp");
  logo = new AnimatedSprite("graphics/logo.bmp", 1, 0, "#000000");
  start = new AnimatedSprite("graphics/start.bmp", 2, 50, "#000000");
  player = new AnimatedSprite("graphics/sprite.bmp", 16, 2, "#000000");
  topRow = new AlienRow(Rank::first);
  upperRow = new AlienRow(Rank::second);
  lowerRow = new AlienRow(Rank::third);
  bottomRow = new AlienRow(Rank::fourth);
  // Set object state
  logo->setLocation({ (settings::SCREEN_WIDTH - logo->getWidth()) / 2, -30 });
  logo->update();
  start->setLocation({(settings::SCREEN_WIDTH - start->getWidth()) / 2, settings::SCREEN_HEIGHT - (2 * player->getHeight() + 30)});
  start->update();
  player->setSpeed(5);
}

void destroyObjects() {
  delete background;
  delete tilemap;
  delete logo;
  delete start;
  delete player;
  delete topRow;
  delete upperRow;
  delete lowerRow;
  delete bottomRow; 
}
