// NOTE: MUST BE COMPILED WITH std=C++14 or higher
// IN CODE BLOCKS DO THE FOLLOWING:
//
//    Project > Build options... > Check the box to compile using g++14
//    ** See further instructions and screenshots in D2L submission and README.md **
//
/***************************** Prologue Section ********************************
 * Title: SDL Invaders - CSC222 Final Project
 * Author: Matthew Kavanagh
 * Date: 6 December, 2024
 * Course & Section: CSC 222-301W
 * Description: This program will render a playable one-player "Space Invaders" game.
 *  The game consists of three rounds of play where the player battles 40 randomly
 *  generated alien ufo enemies. The player gains one point for each alien destroyed,
 *  and loses one of their three lives each time an alien collides with them or the base
 *  they are guarding. Each round advances when all 40 aliens have been cleared. Rounds get 
 *  progressively more difficult. Clear alln3 rounds to win the game.
 *
 * Controls:
 *  Left Key    // Move the player left
 *  Right Key   // Move the player right
 *  Space Key   // Fire a bullet
 *  Esc Key     // End the game
 *
 * Data Requirements:
 *    background.h / background.cpp   // Contain all background objects and functions
 *    engine.h / engine.cpp           // Contain all SDL objects and functions
 *    settings.h                      // Contain global game settings
 *    sprite.h / sprite.cpp           // Contain all objects and functions for animated sprites
 *    types.h / types.cpp             // User-defined datatypes and related functions
 *
 *    ** All headers contain declarations and class definitions, cpp files contain function definitions and initializations
 *
 * Refined Algorithms (see headers for class definitions):
 *  background.cpp:
 *    **Contains background and tilemap functions
 *    **No new functions from in-class
 *
 *  engine.cpp:
 *    **Wrapper for SDL functions
 *    **No new functions from in-class
 *
 *  main.cpp (this file):
 *    int main()
 *    bool game::init()                   // Initialize the game
 *    void game::end()                    // End the game
 *    void game::update()                 // Update all objects
 *    void game::draw()                   // Draw all active objects
 *    void game::nextRound()              // Progress to next round
 *    void game::setMenu(int round)                     // Setup menu for display
 *    void game::displayMenu(const Uint8* pressedKeys)  // Display selected menu until space
 *    void game::displayEnd()    
 *                       // Display ending menu
 *  sprite.cpp:
 *    class AnimatedSprite:
 *      AnimatedSprite()                  // Constructor and derivatives
 *      void nextFrame()                  // Update animation
 *      void draw()                       // Draw sprite to render
 *      void setLocation(const Point2d&)  // Set location for sprite
 *      void setDirection(const Direction&)   // Set the direction for movement
 *      void setSpeed(int)            // Set the sprite movement speed
 *      bool move()                   // Move the sprite in the current direction
 *      void update()                 // Copy sprite's location to the render rectangle
 *    class Alien (inherits AnimatedSprite):
 *      static bool init()          // Initialize static textures
 *      void moveDown()             // Move the alien down a row and flip direction
 *      void destroy()              // Destroy the alien object and prevent it from rendering
 *    class AlienRow:
 *      void resetLocation()        // Reset all aliens to their starting location
 *      void resetRound(int round)           // Set all variables for the current round
 *      void moveDown()                   // Move each alien in the row down
 *      void update()               // Update each alien render position
 *      void draw()                 // Draw each alien to the render
 *    class Bullet (inherits AnimatedSprite):
 *      static bool init()        // Initialize static textures
 *      void moveUp()             // Move the bullet up
 *      void shoot()              // Initially shoot the bullet
 *    struct Bullets:
 *      void update()             // Update the render location of each active bullet
 *      void draw()               // Draw each active bullet
 *      void fire(const sprite&)  // Fire bullet from center of given sprite
 *      bool checkCollisions(alienRow)  // Check collision between active bullets and each alien in row
 *    bool checkCollision(sprite1, sprite2)   // Check for collision between two sprites
 *    void explode(const Point2d&, int)       // Explosion animation at given location and delay time
 *
 *  types.cpp
 *    RGB hexToRGB(std::string hex)   // Covert a hex color value to R,G,B values
 *
 ******************************************************************************/

#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <ostream>
#include <string>
#include <SDL2/SDL.h>
#include "engine.h"
#include "types.h"
#include "sprite.h"
#include "background.h"
#include "settings.h"


/****************************** GLOBAL DATA ***********************************/

// Global Game Variables
int playerScore = 0;    // Player's score
int playerLives = 3;    // Player's lives
int currentRound = 1;   // The current round of play
const int MAX_ROUNDS = 3;   // Maximum number of rounds of play
bool newRound = false;   // Do we need to set up the next round?
bool playGame = false;  // Menu state variable
bool startRound = false; // Start the next round
bool menuDeleted = false;   // Did we delete the menu object?
bool playerWin = false;     // Did the player win?

// Declare Global game objects
// To be intialized in init functions
Background* background = NULL;
Tilemap* tilemap = NULL;
AnimatedSprite* logo = NULL;        // Pointers to all sprite objects
AnimatedSprite* roundOne = NULL;
AnimatedSprite* roundTwo = NULL;
AnimatedSprite* roundThree = NULL;
AnimatedSprite* winLogo = NULL;
AnimatedSprite* loseLogo = NULL;
AnimatedSprite* start = NULL;
AnimatedSprite* player = NULL;
AnimatedSprite* explosion = NULL;
AlienRow* topRow = NULL;    // Alien Rows hold 10 alien sprites
AlienRow* upperRow = NULL;
AlienRow* lowerRow = NULL;
AlienRow* bottomRow = NULL;
Bullets* bullets = NULL;    // Bullets holds 5 bullets to rotate through

// Function Prototypes
// Game state functions
void createObjects();   // Instantiate game objects and set initial states
void createAliens(int speed);   // Create all alien objects
void destroyObjects();  // Free memory associated with instantiated objects and nullify pointers
void deleteAliens();    // Delete all alien objects

// Gameplay Functions
namespace game {
  bool init();  // Initialize all game objects
  void end();   // Destory game objects and end game
  void update();    // Update the state of each object
  void draw();  // Draw each object to the render
  void nextRound(); // Set up environment for next round of play
  void setMenu(int round);      // Set which menu to display between each round
  void displayMenu(const Uint8* pressedKeys);   // Display a start menu
  void displayEnd();    // Display win or lose message at the end
}

/************************** Function: main() *****************************
 * Description: This function will handle the main game loop. It will take in key presses
 *  and handle rendering of menus, and all game objects. The game will also handle all logical checking
 *  for game state.

 * Data Requirements:
 *  const Uint8* keys     // Holds the key presses
 *  
 *
 * Global Variables:
 *  int playerScore       // Player's score
 *  int playerLives       // Player's lives
 *  int currentRound      // The current round of play
 *  const int MAX_ROUNDS; // Maximum number of rounds of play
 *  bool newRound;        // Do we need to set up the next round?
 *  bool playGame;        // Menu state variable
 *  bool startRound;      // Start the next round
 *  bool menuDeleted;     // Did we delete the menu object?
 *  bool playerWin;       // Did the player win?
 *
 * Formulas:
 *  none
 *
 * Refined Algorithm
 *  Print out name
 *  Initialize game variables
 *    IF !init(),
 *    THEN print error message and exit
 *  BEGIN GAME LOOP
 *  While(ProgramIsRunning)
 *    Get key presses from user
 *    IF Escape,
 *      THEN, break from game loop
 *    IF left,
 *      THEN, set player direction to left and move
 *    IF right,
 *      THEN, set player direction to right and move
 *  IF !(playGame)
 *      THEN, display start menu
 *  ELSE PLAY GAME
 *    IF(newRound)
 *      THEN, update variables for next round
 *    IF !(startRound)
 *      THEN, set menu to current round and display menu
 *    ELSE PLAY GAME
 *      Increment bullet timer
 *      IF player pressed space
 *        THEN, fire bullet
 *      Update all game objects
 *      Draw all game objects
 *      Check collisions between active bullets and aliens
 *      Check collisions between player and aliens
 *        IF collide,
 *          THEN, reset sprite locations
 *      Check for player loss
 *        IF(playerLives <= 0) THEN set win to false and break
 *      Check for player win
 *        IF(each row is empty and round is 3) THEN set win to true and break
 *      Else if each row is empty set newRound to true
 *  END GAME LOOP
 *  Display end menu
 *******************************************************************************/

int main(int argc, char* argv[]) {
  std::cout << "Matthew Kavanagh\nCSC-222-301W | Fall 2024\nSUNY OCC\nFinal Project\n\n";
  
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
      if(newRound)
        game::nextRound();

      if(!startRound) {
        // Point the logo pointer to the current round
        game::setMenu(currentRound);
        // And display the menu
        game::displayMenu(keys);
      }
      else {
        
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
        //bottomRow->checkExplode();
        //lowerRow->checkExplode();
        //upperRow->checkExplode();
        //topRow->checkExplode();
        if(bottomRow->checkCollisions(*player) || lowerRow->checkCollisions(*player) || upperRow->checkCollisions(*player) || topRow->checkCollisions(*player)) {
          topRow->resetLocation();
          upperRow->resetLocation();
          lowerRow->resetLocation();
          bottomRow->resetLocation();
        }
        
        // CHECK FOR WIN/LOSS
        // Check if player loses
        if(playerLives <= 0) {
          std::cout << "Player loses\n";
          playerWin = false;
          break;
        }
        // Check if all enemies are destoryed
        if(topRow->isEmpty() && upperRow->isEmpty() && lowerRow->isEmpty() && bottomRow->isEmpty()) {
          // Check if player won
          if(currentRound == MAX_ROUNDS) {
            std::cout << "Player wins!\n";
            playerWin = true;
            break;
          }
          newRound = true;
        }
      }
    }
  }
  // Display end menu and exit
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
  if(playerWin) {
    logo = winLogo;
  }
  else {
    logo = loseLogo;
  }
  game::displayEnd();
  destroyObjects();
  SDL::CloseShop();
}

void createObjects() {
  background = new Background("graphics/bg.bmp");
  tilemap = new Tilemap("graphics/tilemap.bmp");
  logo = new AnimatedSprite("graphics/logo.bmp", 1, 0, "#000000");
  roundOne = new AnimatedSprite("graphics/roundone.bmp", 1, 0, "#000000");
  roundTwo = new AnimatedSprite("graphics/roundtwo.bmp", 1, 0, "#000000");
  roundThree = new AnimatedSprite("graphics/roundthree.bmp", 1, 0, "#000000");
  winLogo = new AnimatedSprite("graphics/win.bmp", 1, 0, "#000000");
  loseLogo = new AnimatedSprite("graphics/lose.bmp", 1, 0, "#000000");
  start = new AnimatedSprite("graphics/start.bmp", 2, 50, "#000000");
  player = new AnimatedSprite("graphics/sprite.bmp", 16, 2, "#000000");
  explosion = new AnimatedSprite("graphics/explosion.bmp", 8, 1, "#000000");
  bullets = new Bullets();
  createAliens(1);

  // Set object location and update render position
  logo->setLocation({ (settings::SCREEN_WIDTH - logo->getWidth()) / 2, -30 });
  logo->update();
  roundOne->setLocation({ (settings::SCREEN_WIDTH - roundOne->getWidth()) / 2, -30 });
  roundOne->update();
  roundTwo->setLocation({ (settings::SCREEN_WIDTH - roundTwo->getWidth()) / 2, -30 });
  roundTwo->update();
  roundThree->setLocation({ (settings::SCREEN_WIDTH - roundThree->getWidth()) / 2, -30 });
  roundThree->update();
  winLogo->setLocation({ (settings::SCREEN_WIDTH - winLogo->getWidth()) / 2, -30 });
  winLogo->update();
  loseLogo->setLocation({ (settings::SCREEN_WIDTH - loseLogo->getWidth()) / 2, -30 });
  loseLogo->update();
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
  delete start;
  delete logo;
  if(playerWin)
    delete loseLogo;
  else
    delete winLogo;
  delete player;
  delete explosion;
  delete bullets;

  // Reset pointers to prevent undefined behavior
  background = NULL;
  tilemap = NULL;
  start = NULL;
  logo = NULL;
  winLogo = NULL;
  loseLogo = NULL;
  player = NULL;
  explosion = NULL;
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

void game::setMenu(int round) {
  // set logo to point to the proper texture
  switch (round) {
    case 1:
      logo = roundOne;
      break;
    case 2:
      logo = roundTwo;
      break;
    case 3:
      logo = roundThree;
      break;
    case 4:
      logo = winLogo;
    case 5:
      logo = loseLogo;
    default:
      logo = NULL;
      break;  
  }
}

/*********** NEEDS HEADER *************/
void game::displayMenu(const Uint8* pressedKeys) {
  // Set the window title
  char title[64];
  std::sprintf(title, "Player Score: %d    |    Lives Remaining: %d", playerScore, playerLives);
  SDL_SetWindowTitle(SDL::gameWindow, title);
  
  bool end = false;
  // Check if player presses start
  if(pressedKeys[SDL_SCANCODE_SPACE]) {
    if(playGame)    // Check if we are entering before game start or before round start
      startRound = true;
    else
      playGame = true;

    SDL_Delay(200);
    // Reset player position
    player->setLocation({ (settings::SCREEN_WIDTH - player->getWidth()) / 2, (settings::SCREEN_HEIGHT - player->getHeight()) - 10 });
    end = true;
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
  if(end) { // Check if we are exiting the function loop
    // Free up logo memory and assign ptr to NULL
    delete logo;
    logo = NULL;
  }
  SDL_Delay(20);
}

void game::displayEnd() {
  // Set the window title
  char title[64];
  std::sprintf(title, "Player Score: %d    |    Lives Remaining: %d", playerScore, playerLives);
  SDL_SetWindowTitle(SDL::gameWindow, title);
  std::cout << "Final Score: " << playerScore << "\nLives Remaining: " << playerLives << std::endl;
  
  int count = 0;
  while(count < 200) {
    player->nextFrame();
    player->setLocation({ (settings::SCREEN_WIDTH - player->getWidth()) / 2, (settings::SCREEN_HEIGHT - player->getHeight()) - 10 });
    player->update();
    background->scroll();
    SDL_RenderClear(SDL::renderer);
    background->draw();
    logo->draw();
    player->draw();
    SDL_RenderPresent(SDL::renderer);
    count++;
    SDL_Delay(20);
  }
}

void game::nextRound() {
  // Increment round counter and reset newRound flag
  currentRound++;
  newRound = false;
  startRound = false;

  //Increse BG speed
  background->scrollSpeed += 2;

  //Reset alien location, state, etc
  topRow->resetRound(currentRound);
  upperRow->resetRound(currentRound);
  lowerRow->resetRound(currentRound);
  bottomRow->resetRound(currentRound);


  // Reset player position
  player->setLocation({ (settings::SCREEN_WIDTH - player->getWidth()) / 2, (settings::SCREEN_HEIGHT - player->getHeight()) - 10 });
}
