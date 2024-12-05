#include "sprite.h"
#include "settings.h"
#include "engine.h"
#include "types.h"
#include <string>
#include <SDL2/SDL.h>
#include <cassert>

// Static alien texture
SDL_Texture* alienTextureSheet = NULL;      // Texture sheet to share for all alien objects
std::string alienSheetPath = "graphics/ufos.bmp";
std::string alienTransparency = "#000000";

// Static bullet texture
SDL_Texture* bulletTextureSheet = NULL;
std::string bulletSheetPath = "graphics/bullet.bmp";
std::string bulletTransparency = "#000000";
int bulletCounter = 4;
const int BULLET_WAIT = 40;
int bulletTimer = BULLET_WAIT;

/*** AnimatedSprite Functions ***/
// Constructors
AnimatedSprite::AnimatedSprite(std::string filePath, int frames, int frameDelay, const RGB& transparencyColor)
  : PATH{ filePath }, MAX_SPRITE_FRAME{ frames }, FRAME_DELAY{ frameDelay }, transparency{ transparencyColor }
{
  SDL::tempSurface = SDL::loadImage(PATH);  // Load the imahe
  SDL::tempSurface = SDL::setTransparentColor(SDL::tempSurface, transparency.r, transparency.g, transparency.b); // Set the surface transparency
  textureSheet = SDL::loadTexture(SDL::tempSurface); // Load the surface onto the texture
  SDL_QueryTexture(textureSheet, NULL, NULL, &rectSheet.w, &rectSheet.h); // Get the dimensions of the texture
  width = rectSheet.w / MAX_SPRITE_FRAME; // Calculate width of a single sprite
  height = rectSheet.h; // Height is equal to sheet height
  position.x = (settings::SCREEN_WIDTH - width) / 2;  // Set initial starting position to bottom center of screen
  position.y = (settings::SCREEN_HEIGHT - height) - 10;
  SDL::FillRect(rectSource, 0, 0, width, height);  // Create the source rectangle to render from
  SDL::FillRect(rectPlacement, position.x, position.y, width, height); // Create the destination render rectangle
}

AnimatedSprite::AnimatedSprite(std::string filePath, int frames, int frameDelay, std::string transparencyHex)
  : AnimatedSprite(filePath, frames, frameDelay, hexToRGB(transparencyHex))
{}   // Convert the hex value to an RGB object and call the constructor as normal

AnimatedSprite::AnimatedSprite(std::string filePath, int frames, int frameDelay)  // Protected helper constructor for alien sprites
  : PATH{ filePath }, MAX_SPRITE_FRAME{ frames }, FRAME_DELAY{ frameDelay }  // Sets const instance data members, the rest is handled by child constructor
{
  // Ensure static members have been initialized before constructing object
  assert((SDL::static_init == true) && "Fatal Error: Tried to create sprite object before initializing static members.");
}

// Destructor
AnimatedSprite::~AnimatedSprite() {
  SDL_DestroyTexture(textureSheet);   //Destroy the texture sheet
}

// Advance to the next frame in animation strip
void AnimatedSprite::nextFrame(){
  frameCounter++; //Increment the frame delay counter

  if(frameCounter > FRAME_DELAY) {
    frameCounter = 0;   //If we reached the delay time
      spriteFrame++;  //Reset the counter
    }

  if(spriteFrame > MAX_SPRITE_FRAME - 1)  //If we've reached the last frame
    spriteFrame = 0;  //Reset to the first frame

  rectSource.x = spriteFrame * width; //Store the current frame's x-location in the source rectangle
}

// Draw current sprite frame to render
void AnimatedSprite::draw() {
  SDL_RenderCopy(SDL::renderer, textureSheet, &rectSource, &rectPlacement);
}

void AnimatedSprite::setLocation(const Point2d& location){
  position = location;  // Set position to the given location
}

void AnimatedSprite::setDirection(const Direction& direction) {
  movementDir = direction;
}

void AnimatedSprite::setSpeed(const int speed) {
  SPEED = speed;
}

bool AnimatedSprite::move() {
  position.x += SPEED * static_cast<int>(movementDir);
  if(position.x <= 0) { //If we hit the left edge
    position.x = 0;     // Bounce off the edge
    return false;       // False means we hit a wall
  }
  if(position.x >= (settings::SCREEN_WIDTH - width)) {
    position.x = settings::SCREEN_WIDTH - width;
    return false;
  }
  return true;
}

void AnimatedSprite::update() {
  rectPlacement.x = position.x;
  rectPlacement.y = position.y;
}

/*** Alien Functions ***/
Alien::Alien()
  : AnimatedSprite(alienSheetPath, 2, std::rand() % 50 + 30)
{
  transparency = hexToRGB(alienTransparency); // Set the transparency member variable
  textureSheet = alienTextureSheet; // Set the texture pointer to point at the alienTextureSheet
  SDL_QueryTexture(textureSheet, NULL, NULL, &rectSheet.w, &rectSheet.h);
  width = rectSheet.w / MAX_SPRITE_FRAME;   // Get the width of a single sprite
  height = rectSheet.h / int(Color::MAX_COLORS);  // Get the height of a single sprite
  // Set a random sprite color
  color = Color(std::rand() % int(Color::MAX_COLORS));  // Color is a random value from 0 to 1-max_colors
  // Set a random animation frame
  spriteFrame = std::rand() % MAX_SPRITE_FRAME; // And we randomize the starting frame
  // Set source rectangle positioning based on animation data
  int sourceX = spriteFrame * width;
  int sourceY = int(color) * height;
  // And fill the source and dest rectangles
  SDL::FillRect(rectSource, sourceX, sourceY, width, height);
  SDL::FillRect(rectPlacement, position.x, position.y, width, height);
}

bool Alien::init(){
  //Initialize the static alien texture
  RGB color = hexToRGB(alienTransparency);
  SDL::tempSurface = SDL::loadImage(alienSheetPath);
  SDL::tempSurface = SDL::setTransparentColor(SDL::tempSurface, color.r, color.g, color.b);
  alienTextureSheet = SDL::loadTexture(SDL::tempSurface);
  if(alienTextureSheet == NULL) {
    std::cout << "Failed to intitialize static alien texture!\n";
    return false;
  }

  SDL::alien_init = true;  // Mark static initialization as completed
  return true;
}

void Alien::moveDown(){
  position.y += (height + AlienRow::GAP_SIZE);
}


AlienRow::AlienRow(Rank position)
  : aliens{
      Alien(),
      Alien(),
      Alien(),
      Alien(),
      Alien(),
      Alien(),
      Alien(),
      Alien(),
      Alien(),
      Alien()
    },
    RANK{position}
{
  //Set to intitial location
  resetLocation();
}

void AlienRow::resetLocation() {
  //Iterate over the row
  int xPos = GUTTER_SIZE;
  int yPos = ((aliens[0].getHeight() + GAP_SIZE) * static_cast<int>(RANK)) + GAP_SIZE;
  for(int i = 0; i < SIZE; ++i) {
    aliens[i].setLocation({xPos, yPos});
    xPos += aliens[i].getWidth() + GAP_SIZE;
  }
  //Set the direction
  if(static_cast<int>(RANK) % 2 == 0) {  // If we have an even rank move right
    xDir = Direction::right;
  }
  else {
    xDir = Direction::left;
  }
}

void AlienRow::moveDown() {
  for(int i = 0; i < SIZE; ++i) {
    aliens[i].moveDown();
  }
}

void AlienRow::update(){
  for(int i = 0; i < SIZE; ++i) {
    aliens[i].setDirection(xDir);
    aliens[i].nextFrame();
    if(!(aliens[i].move())) { // If we collide with a wall flip direction and move down
      if(xDir == Direction::right)
        xDir = Direction::left;
      else
        xDir = Direction::right;
      // Move the entire row down
      moveDown();
    }
    aliens[i].update();
  }
}

void AlienRow::draw(){
  for(int i = 0; i < SIZE; ++i) {
    aliens[i].draw();
  }
}

bool AlienRow::checkCollisions(const AnimatedSprite& playerSprite){
  if(!empty) {  // If the row is not empty
    for(int i = 0; i < SIZE; ++i) {   // For each alien in the row
      if(checkCollision(aliens[i], playerSprite) || (aliens[i].getLocation().y + aliens[i].getHeight()) >= 25*32) { // If the player collides with the sprite or sprite reaches the player base
        std::cout << "Alien collided with player, lose a life.\n";
        // Decrement player life
        // Reset positioning
        // Return true, we don't need to check any further
      }
    }
  }
  return false;
}

Bullet::Bullet() 
  : AnimatedSprite(bulletSheetPath, 1, 0)
{
  transparency = hexToRGB(bulletTransparency); // Set the transparency member variable
  textureSheet = bulletTextureSheet; // Set the texture pointer to point at the alienTextureSheet
  SDL_QueryTexture(textureSheet, NULL, NULL, &rectSheet.w, &rectSheet.h);
  width = rectSheet.w / MAX_SPRITE_FRAME;   // Get the width of a single sprite
  height = rectSheet.h;  // Get the height of a single sprite
    // Set source rectangle positioning based on animation data
  int sourceX = 0;
  int sourceY = 0;
  setLocation({-100, -100});
  setSpeed(15);
  active = false;
  // And fill the source and dest rectangles
  SDL::FillRect(rectSource, sourceX, sourceY, width, height);
  SDL::FillRect(rectPlacement, position.x, position.y, width, height);
}

// Initialize static bullet members
bool Bullet::init(){
  //Initialize the static alien texture
  RGB color = hexToRGB(bulletTransparency);
  SDL::tempSurface = SDL::loadImage(bulletSheetPath);
  SDL::tempSurface = SDL::setTransparentColor(SDL::tempSurface, color.r, color.g, color.b);
  bulletTextureSheet = SDL::loadTexture(SDL::tempSurface);
  if(bulletTextureSheet == NULL) {
    std::cout << "Failed to intitialize static bullet texture!\n";
    return false;
  }

  SDL::bullet_init = true;  // Mark static initialization as completed
  return true;
}

void Bullet::shoot() {
  // Set the bullet to active
  active = true;
  // And update it's position
  update();
}

// Move the bullet upward by SPEED
// No change to xPosition values
void Bullet::moveUp(){
  position.y -= SPEED;
  // Check if off the screen
  if(position.y <= (height * -1)) {
    active = false;
  }
  // And update render location
  update();
}

Bullets::Bullets() 
  : armory{
    Bullet(),
    Bullet(),
    Bullet(),
    Bullet(),
    Bullet()
  }
{}

void Bullets::fire(const AnimatedSprite& player) {
  if(bulletTimer >= BULLET_WAIT) {
    int xPos = player.getLocation().x + (player.getWidth() - armory[bulletCounter].getWidth()) / 2;
    int yPos = player.getLocation().y - armory[bulletCounter].getHeight();
    armory[bulletCounter].setLocation({xPos, yPos});
    armory[bulletCounter].shoot();
    bulletCounter++;
    if(bulletCounter >= MAX_ACTIVE) {
      bulletCounter = 0;
    }
    bulletTimer = 0;
    std::cout << "Firing bullet #" << bulletCounter + 1 << '\n';
  }
}

void Bullets::update() {
  for(int i = 0; i < MAX_ACTIVE; ++i) {
    if(armory[i].isActive()) {
      armory[i].moveUp();
    }
  }
}

void Bullets::draw() {
  for(int i = 0; i < MAX_ACTIVE; ++i) {
    if(armory[i].isActive()) {
      armory[i].draw();
    }
  }
}

bool Bullets::checkCollisions(AlienRow& topRow, AlienRow& upperRow, AlienRow& lowerRow, AlienRow& bottomRow){
  for(int i = 0; i < MAX_ACTIVE; ++i) { // For each bullet
    if(armory[i].isActive()) {  // If the bullet is currently active
      if(!bottomRow.isEmpty()) {  // IF bottom row is not empty
        for(int j = 0; j < bottomRow.SIZE; j++) {   // For each alien in bottom row
          if(checkCollision(armory[i], bottomRow.aliens[j])) {  // If the alien and bullet collide
            std::cout << "Collision between bullet #" << i + 1 << " and a " << static_cast<int>(bottomRow.aliens[j].getColor()) << " ufo.\n";
            return true;
            // Destory and explode the alien
            // increment the score
            // mark the bullet as not active
            // Check and update row empty status
            // return true, we don't need to check any further
          }
        }
      }
      if(!lowerRow.isEmpty()) {  // IF bottom row is not empty
        for(int j = 0; j < lowerRow.SIZE; j++) {   // For each alien in bottom row
          if(checkCollision(armory[i], lowerRow.aliens[j])) {  // If the alien and bullet collide
            std::cout << "Collision between bullet #" << i + 1 << " and a " << static_cast<int>(lowerRow.aliens[j].getColor()) << " ufo.\n";
            return true;
            // Destory and explode the alien
            // increment the score
            // mark the bullet as not active
            // Check and update row empty status
            // return true, we don't need to check any further
          }
        }
      }
      if(!upperRow.isEmpty()) {  // IF bottom row is not empty
        for(int j = 0; j < upperRow.SIZE; j++) {   // For each alien in bottom row
          if(checkCollision(armory[i], upperRow.aliens[j])) {  // If the alien and bullet collide
            std::cout << "Collision between bullet #" << i + 1 << " and a " << static_cast<int>(upperRow.aliens[j].getColor()) << " ufo.\n";
            return true;
            // Destory and explode the alien
            // increment the score
            // mark the bullet as not active
            // Check and update row empty status
            // return true, we don't need to check any further
          }
        }
      }
      if(!topRow.isEmpty()) {  // IF bottom row is not empty
        for(int j = 0; j < topRow.SIZE; j++) {   // For each alien in bottom row
          if(checkCollision(armory[i], topRow.aliens[j])) {  // If the alien and bullet collide
            std::cout << "Collision between bullet #" << i + 1 << " and a " << static_cast<int>(topRow.aliens[j].getColor()) << " ufo.\n";
            return true;
            // Destory and explode the alien
            // increment the score
            // mark the bullet as not active
            // Check and update row empty status
            // return true, we don't need to check any further
          }
        }
      }
    }
  }
  return false;
}

// Override Operator<< to print to screen
// Print sprite info to the screen
std::ostream& operator<<(std::ostream& out, const AnimatedSprite& sprite) {
  out << "Sprite:\n"
    << "Path: \'" << sprite.PATH << "\'\n"
    << "Texture Ptr: " << sprite.textureSheet << '\n'
    << "Transparency: " << sprite.transparency << '\n'
    << "Dimensions (w,h): " << sprite.width << ", " << sprite.height << '\n'
    << "Position: " << sprite.position << '\n'
    << "Rectangle (x,y,w,h):\n"
    << "  Source: " << static_cast<int>(sprite.rectSource.x) << ", " << static_cast<int>(sprite.rectSource.y) << ", " << static_cast<int>(sprite.rectSource.w) << ", " << static_cast<int>(sprite.rectSource.h) << '\n'
    << "  Placement: " << static_cast<int>(sprite.rectPlacement.x) << ", " << static_cast<int>(sprite.rectPlacement.y) << ", " << static_cast<int>(sprite.rectPlacement.w) << ", " << static_cast<int>(sprite.rectPlacement.h) << '\n'
    << "Animation:\n"
    << "  Current Frame: " << sprite.spriteFrame + 1 << " / " << sprite.MAX_SPRITE_FRAME << '\n'
    << "  Delay: " << sprite.frameCounter << " / " << sprite.FRAME_DELAY << '\n'
    << "Explosion Animation State: " << sprite.isExploded << '\n'
    << "Destroyed State: " << sprite.isDestroyed;

  return out;
}

// Check for collision between two sprites
// Returns false if no collision occured or true if they collide
bool checkCollision(const AnimatedSprite& sprite1, const AnimatedSprite& sprite2) {
    if(sprite1.getLocation().x >= sprite2.getLocation().x + sprite2.getWidth())
        return false;
    if(sprite1.getLocation().y >= sprite2.getLocation().y + sprite2.getHeight())
        return false;
    if(sprite2.getLocation().x >= sprite1.getLocation().x + sprite1.getWidth())
        return false;
    if(sprite2.getLocation().y >= sprite1.getLocation().y + sprite1.getHeight())
        return false;
    return true;
}

// Print info for an Alien object
std::ostream& operator<<(std::ostream& out, const Alien& alien) {
  out << "Alien " << static_cast<const AnimatedSprite&>(alien) << '\n'
    << "UFO Color: ";

  switch(alien.color) {
    case Alien::Color::green:
      out << "green";
      break;
    case Alien::Color::blue:
      out << "blue";
      break;
    case Alien::Color::gray:
      out << "gray";
      break;
    case Alien::Color::pink:
      out << "pink";
      break;
    case Alien::Color::purple:
      out << "purple";
      break;
    case Alien::Color::yellow:
      out << "yellow";
      break;
    case Alien::Color::brown:
      out << "brown";
      break;
    case Alien::Color::red:
      out << "red";
      break;
    case Alien::Color::orange:
      out << "orange";
      break;
    default:
      out << "UNDEF";
      break;
  }
  out << '(' << static_cast<int>(alien.color) <<')';
  return out;
}

// Print a row of aliens
std::ostream& operator<<(std::ostream& out, const AlienRow& row) {
  out << "Alien Row [" << row.SIZE << "]\n";
  for(int i = 0; i <row.SIZE; ++i){
    out << '*' << row.aliens[i] << '\n';
  }

  return out;
}

// Print a bullet object
std::ostream& operator<<(std::ostream& out, const Bullet& bullet) {
  out << "Bullet " << static_cast<const AnimatedSprite&>(bullet);
  return out;
}
