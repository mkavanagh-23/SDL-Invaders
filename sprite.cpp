#include "sprite.h"
#include "settings.h"
#include "engine.h"
#include <string>
#include <SDL2/SDL.h>
#include <cassert>

// Static alien texture
SDL_Texture* alienTextureSheet = NULL;      // Texture sheet to share for all alien objects
std::string alienSheetPath = "graphics/ufos.bmp";
std::string alienTransparency = "#000000";

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
Alien::Alien(std::string filePath, int frames, int frameDelay, const RGB& transparencyColor)
  : AnimatedSprite(filePath, frames, frameDelay)
{
  transparency = transparencyColor; // Set the transparency member variable
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

Alien::Alien(std::string filePath, int frames, int frameDelay, std::string transparencyHex)
  : Alien(filePath, frames, frameDelay, hexToRGB(transparencyHex))
{}

bool Alien::init(){
  //Initialize the static alien texture
  RGB color = hexToRGB(alienTransparency);
  SDL::tempSurface = SDL::loadImage(alienSheetPath);
  SDL::tempSurface = SDL::setTransparentColor(SDL::tempSurface, color.r, color.g, color.b);
  alienTextureSheet = SDL::loadTexture(SDL::tempSurface);
  if(alienTextureSheet == NULL) {
    std::cout << "Failed to intitialize static textures!\n";
    return false;
  }

  SDL::static_init = true;  // Mark static initialization as completed
  return true;
}

void Alien::moveDown(){
  position.y += (height + AlienRow::GAP_SIZE);
}

AlienRow::AlienRow(Rank position)
  : aliens{
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency),
      Alien(alienSheetPath, 2, std::rand() % 50 + 30, alienTransparency)
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
