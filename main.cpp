#include <SDL2/SDL_video.h>
#include <cstdio>
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

// COMPILE WITH --std=c++14

// TODO:
// Set up scoring
  // For each UFO destroyed, playerScore++
  // 40 total ufos... can use either score / 40 to determine round (likely faster)
  // or we can check each row for empty state (likely slower but more logical)
// Set up lives
  // 3 Lives
  // or do we want to just want to check collision with the player sprite and 'y + height' with the y-axis?
  // Upon lost life, round continues but enemy posititions are reset (destroyed stay destroyed)
// Bind enemy speed to background scroll speed and increment with each round
// The increased scroll speed gives a more stressful feel

/****************************** GLOBAL DATA ***********************************/

// Global Game Variables
int playerScore = 0;    // Player's score
int playerLives = 3;    // Player's lives
//int currentRound = 1;
//bool newRound = false;   // Are we at the start of a new round?
bool playGame = false;  // Menu state variable

// Declare Global game objects
Background* background = NULL;
Tilemap* tilemap = NULL;
AnimatedSprite* logo = NULL;
AnimatedSprite* start = NULL;
AnimatedSprite* player = NULL;
AlienRow* topRow = NULL;    // Alien Rows hold 10 alien sprites
AlienRow* upperRow = NULL;
AlienRow* lowerRow = NULL;
AlienRow* bottomRow = NULL;
Bullets* bullets = NULL;    // Bullets holds 5 bullets to rotate through

// Function Prototypes
// Game state functions
void createObjects();   // Instantiate game objects and set initial states
void createAliens(int speed);
void destroyObjects();  // Free memory associated with instantiated objects and nullify pointers
void deleteAliens();

// Gameplay Functions
namespace game {
  bool init();  // Initialize all game objects
  void end();   // Destory game objects and end game
  void update();    // Update the state of each object
  void draw();  // Draw each object to the render
//  void nextRound();
  void displayMenu(const Uint8* pressedKeys);   // Display a start menu
}

/************************* MAIN GAME FUNCTION ***************************/
int main(int argc, char* argv[]) {
  
  // Initialize libraries and static data members
  // Check for successful initialization, exit if it failed
  if(!game::init()) {
    std::cout << "Critical error, terminating program\n";
    return 1;
  }

  // BEGIN GAME LOOP
  while(SDL::ProgramIsRunning()) {
    // Get key press from keyboard and interpret
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    // End game at any time with 'ESC'
    if(keys[SDL_SCANCODE_ESCAPE])
      break;

    // Check for left and right arrow keypresses
    // Move the player sprite accordingly
    // This is outside the game loop so the player can have fun before pressing start
    if(keys[SDL_SCANCODE_LEFT]) {
      player->setDirection(Direction::left);
      player->move();
    }
    if(keys[SDL_SCANCODE_RIGHT]) {
      player->setDirection(Direction::right);
      player->move();
    }

    // Display a menu until the player quits or selects 'START' (Space Key)
    if(!playGame) {
      game::displayMenu(keys);
    }
    
    // Play the game 
    else {
      // Check for new round
      //if(newRound)
      //  game::nextRound();
      
      // Check for Space key press
      // If pressed, fire a bullet from the player sprite
      bulletTimer++;        // Increment the bullet timer to see if we can fire this tick
      if(keys[SDL_SCANCODE_SPACE]) {
        bullets->fire(*player);
      }
      
      // Update state of all game objects before drawing
      game::update();
      
      // Draw all active objects to the render
      game::draw();

      // CHECK COLLISIONS!!
      bullets->checkCollisions(*bottomRow);
      bullets->checkCollisions(*lowerRow);
      bullets->checkCollisions(*upperRow);
      bullets->checkCollisions(*topRow);
      if(bottomRow->checkCollisions(*player) || lowerRow->checkCollisions(*player) || upperRow->checkCollisions(*player) || topRow->checkCollisions(*player)) {
        topRow->resetLocation();
        upperRow->resetLocation();
        lowerRow->resetLocation();
        bottomRow->resetLocation();
      }
      
      // CHECK FOR WIN/LOSS
      // CHECK FOR ALL ENEMIES DESTROYED

    }
  }

  game::end();
  return 0;
}

bool game::init() {
  //Initialize SDL
  if(!SDL::Init())
    return false;

  //Initialize static textures
  if(!Alien::init() || !Bullet::init()) {
    return false;
  }
  SDL::static_init = true;

  //Create game objects
  createObjects();

  return true;  // If we made it this far then we initialized successfully
}

void game::end() {
  destroyObjects();
  SDL::CloseShop();
}

void createObjects() {
  background = new Background("graphics/bg.bmp");
  tilemap = new Tilemap("graphics/tilemap.bmp");
  logo = new AnimatedSprite("graphics/logo.bmp", 1, 0, "#000000");
  start = new AnimatedSprite("graphics/start.bmp", 2, 50, "#000000");
  player = new AnimatedSprite("graphics/sprite.bmp", 16, 2, "#000000");
  bullets = new Bullets();
  createAliens(1);

  // Set object state
  logo->setLocation({ (settings::SCREEN_WIDTH - logo->getWidth()) / 2, -30 });
  logo->update();
  start->setLocation({(settings::SCREEN_WIDTH - start->getWidth()) / 2, settings::SCREEN_HEIGHT - (2 * player->getHeight() + 30)});
  start->update();
  player->setSpeed(5);
}

void createAliens(int speed) {
  topRow = new AlienRow(Rank::first, speed);
  upperRow = new AlienRow(Rank::second, speed);
  lowerRow = new AlienRow(Rank::third, speed);
  bottomRow = new AlienRow(Rank::fourth, speed);
}

void destroyObjects() {
  deleteAliens();
  // Delete each object
  delete background;
  delete tilemap;
  delete logo;
  delete start;
  delete player;
  delete bullets;

  // Reset pointers to prevent undefined behavior
  background = NULL;
  tilemap = NULL;
  logo = NULL;
  start = NULL;
  player = NULL;
  bullets = NULL;
}

void deleteAliens() {
  delete topRow;
  delete upperRow;
  delete lowerRow;
  delete bottomRow;
  topRow = NULL;
  upperRow = NULL;
  lowerRow = NULL;
  bottomRow = NULL; 

}

void game::update() {
    // Advance to next frame
    player->nextFrame();
    background->scroll();

    // Update each object
    player->update();
    topRow->update();
    upperRow->update();
    lowerRow->update();
    bottomRow->update();
    bullets->update();
}

void game::draw() {
    // Set the window title
    char title[64];

    std::sprintf(title, "Player Score: %d    |    Lives Remaining: %d", playerScore, playerLives);
    SDL_SetWindowTitle(SDL::gameWindow, title);
    
    SDL_RenderClear(SDL::renderer);
    background->draw();
    tilemap->draw();
    player->draw();
    topRow->draw();
    upperRow->draw();
    lowerRow->draw();
    bottomRow->draw();
    bullets->draw();
    SDL_RenderPresent(SDL::renderer);
    SDL_Delay(20);
}

/*********** NEEDS HEADER *************/
void game::displayMenu(const Uint8* pressedKeys) {
  // Check if player presses start
  if(pressedKeys[SDL_SCANCODE_SPACE]) {
    playGame = true;
    SDL_Delay(200);
    // Reset player position
    player->setLocation({ (settings::SCREEN_WIDTH - player->getWidth()) / 2, (settings::SCREEN_HEIGHT - player->getHeight()) - 10 });
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
}

// void game::nextRound() {
//   // Increment round counter and reset newRound flag
//   currentRound++;
//   newRound = false;
// 
//   // Set alien speed for current round
//   int alienSpeed;
//   switch (currentRound) {
//     case 1:
//       alienSpeed = 2;
//     case 2:
//       alienSpeed = 3;
//     case 3:
//       alienSpeed = 4;
//     default:
//       alienSpeed = 1;
//   }
// 
//   // Delete alien rows
//   deleteAliens();
//   // And regenerate new ones
//   createAliens();
//   
//   // Reset player position
//   player->setLocation({ (settings::SCREEN_WIDTH - player->getWidth()) / 2, (settings::SCREEN_HEIGHT - player->getHeight()) - 10 });
// 
//   // Wait before starting round
// }
