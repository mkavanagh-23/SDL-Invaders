#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <ostream>
#include <string>
#include <SDL2/SDL.h>
#include "../include/engine.h"
#include "../include/types.h"
#include "../include/sprite.h"
#include "../include/background.h"
#include "../include/settings.h"


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
  void drawExplosion();
}

// Begin main function
int main(int argc, char* argv[]) {
  // Print out directions
  std::cout << "Directions:\n";
  std::cout << "  Your spaceship is under attack! Fend off the invading waves of alien\n"
            << "    invaders coming to attack your ship.\n";
  std::cout << "  Gameplay consists of three rounds of invading aliens, each at an increasing speed.\n"
            << "  For each alien destroyed, the player gains one point. However, if an alien reaches the player or their base,\n"
            << "    the player loses a life. The player only has 3 lives.\n"
            << "  Survive all 3 rounds to win!\n\n";
  std::cout << "Controls:\n";
  std::cout << "  LEFT/RIGHT KEYS:   Move spaceship left/right\n"
            << "  SPACE KEY:         Fire a bullet\n"
            << "  ESC KEY:           End the game at any time\n\n";
  std::cout << "Good luck and have fun!\n" << std::endl;

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

// Initialize all objects
bool game::init() {
  //Initialize SDL
  if(!SDL::Init())
    return false;

  //Initialize static textures
  if(!Alien::init() || !Bullet::init()) {
    return false;
  }
  SDL::static_init = true;  // Set static initialization flag to true

  //Create game objects
  createObjects();

  return true;  // If we made it this far then we initialized successfully
}

// End the game
void game::end() {
  // If player wins set the logo to win logo
  if(playerWin) {
    logo = winLogo;
  }
  // Otherwise set to lose logo
  else {
    logo = loseLogo;
  }
  // Display the end menu
  game::displayEnd();
  // Destroy all objects
  destroyObjects();
  // Destroy SDL objects and end session
  SDL::CloseShop();
}

// Instantiate all objects with initial values
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
  explosion->isActive = false;
}

// Instantiate all alien objects with given speed
void createAliens(int speed) {
  topRow = new AlienRow(Rank::first, speed);
  upperRow = new AlienRow(Rank::second, speed);
  lowerRow = new AlienRow(Rank::third, speed);
  bottomRow = new AlienRow(Rank::fourth, speed);
}

// Destory all objects and assign pointers to NULL to prevent invalid memory access
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

// Delete all alien objects and assign pointers to NULL
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

// Update all object animations and update render locations
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
    if(explosion->atEnd()) {
      explosion->isActive = false;
    }
    if(explosion->isActive) {
      explosion->nextFrame();
      explosion->update();
    }
}

void game::draw() {
    // Set the window title
    char title[64];
    std::sprintf(title, "Player Score: %d    |    Lives Remaining: %d", playerScore, playerLives);
    SDL_SetWindowTitle(SDL::gameWindow, title);
    
    // Draw the frame
    SDL_RenderClear(SDL::renderer);
    background->draw();
    tilemap->draw();
    player->draw();
    topRow->draw();
    upperRow->draw();
    lowerRow->draw();
    bottomRow->draw();
    bullets->draw();
    drawExplosion();
    SDL_RenderPresent(SDL::renderer);
    SDL_Delay(20);
}

void game::drawExplosion() {
  if(explosion->isActive) { // Check if explosion has been triggered
    explosion->draw(); 
  }
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
    default:
      logo = NULL;
      break;  
  }
}

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

// SAME AS ABOVE WITHOUT GAME START/ROUND CHECK
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
