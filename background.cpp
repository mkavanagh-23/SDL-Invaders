#include "background.h"
#include "engine.h"
#include "settings.h"
#include <fstream>

// Constructor
Background::Background(std::string filePath)
  : PATH{ filePath }
{
  SDL::tempSurface = SDL::loadImage(PATH);    //load the image
  texture = SDL::loadTexture(SDL::tempSurface);   //Load the image into a texture
  SDL::FillRect(rect, 0, 0, settings::SCREEN_WIDTH, settings::SCREEN_HEIGHT);    //Create a rendering rectangle
}

// Destructor
Background::~Background() {
  SDL_DestroyTexture(texture);  // Destroy the texture object
}

// Increment BG by scrollSpeed
void Background::scroll() {
  yOffset += scrollSpeed;   //Increment the y-offset by the scroll speed
  if(yOffset >= settings::SCREEN_HEIGHT)  //If the image has moved off the screen
    yOffset = 0;    //Reset the position
  rect.y = yOffset; //And store the new value in the render rectangle
}

void Background::speedUp(int speedIncrease) {
  scrollSpeed += speedIncrease;
}

// Draw the background to the render
void Background::draw() {
  SDL_RenderCopy(SDL::renderer, texture, NULL, &rect); // Copy the image to the render
  rect.y = yOffset - settings::SCREEN_HEIGHT; // Scroll the image down
  SDL_RenderCopy(SDL::renderer, texture, NULL, &rect); //Copy the image to the render
}

Tilemap::Tilemap(std::string filePath)
  : PATH{filePath}
{
  // Set up the texture
  // Load the image onto a transoarent surface
  SDL::tempSurface = SDL::loadImage("graphics/tiles.bmp");
  SDL::tempSurface = SDL::setTransparentColor(SDL::tempSurface, 0, 255, 0);
  // Load the surface onto a texture
  texture = SDL::loadTexture(SDL::tempSurface);

  // Get the tile data  
  // Load the map file
  std::ifstream in("graphics/map.map");

  if(!in.good())
    std::cout << "The map did not load.\n";

  std::string buffer;

  // Get the line that says 'destWidth'
  std::getline(in, buffer, ' ');
  // Get the width value
  std::getline(in, buffer, '\n');
  sheetWidth = std::atoi(buffer.c_str());

  // Get the line that says 'destHeight'
  std::getline(in, buffer, ' ');
  // Get the height value
  std::getline(in, buffer, '\n');
  sheetHeight = std::atoi(buffer.c_str());

  // Get the line that says 'mapWidth'
  std::getline(in, buffer, ' ');
  // Get the mapWidth value
  std::getline(in, buffer, '\n');
  mapCol = std::atoi(buffer.c_str());

  // Get the line that says 'mapHeight'
  std::getline(in, buffer, ' ');
  // Get the mapHeight value
  std::getline(in, buffer, '\n');
  mapRow = std::atoi(buffer.c_str());

  // Get the line that says 'tile_width'
  std::getline(in, buffer, ' ');
  // Get the tile_width value
  std::getline(in, buffer, '\n');
  tileWidth = std::atoi(buffer.c_str());

  // Get the line that says 'tile_height'
  std::getline(in, buffer, ' ');
  // Get the tile_height value
  std::getline(in, buffer, '\n');
  tileHeight = std::atoi(buffer.c_str());
  
  // Set up the array to hold the tiles
  tiles = new int[mapCol * mapRow];

  // Get the line that says 'layer1'
  std::getline(in, buffer, '\n');

  int i = 0;    // array index
  for(int row = 0; row < mapRow; row++) {
    for(int col = 0; col < mapCol; col++) {
      char delim = ','; // comma delimits between frames
      if(col == mapCol - 1) // Unless last frame in row, then set to new line
        delim = '\n';
      std::getline(in, buffer, delim);  // Get the Frame ID
      int data = std::atoi(buffer.c_str());     // Convert it to an integer
      tiles[i] = data;      // And store the Frame ID at the tile's index
      i++;
    }   // End column loop
  } // End row loop

  in.close();
}

Tilemap::~Tilemap(){
  SDL_DestroyTexture(texture);
  if(tiles != NULL) {
    delete[] tiles;
    tiles = NULL;
  }
}

void Tilemap::draw(){
  // Draw a layer
  for(int row = 0; row < mapRow; row++) {
    for(int col = 0; col < mapCol; col++) {
      int frame = tiles[row*mapCol+col];    // Get the frame id of the current tile
      if(frame > 0) {   // Grab the tile if we have a Frame ID
        // Calculate x and y location of tile on sheet
        int rawBytes = ((frame -1) * tileWidth);
        int xSource = rawBytes % sheetWidth;
        int ySource = (rawBytes / sheetWidth) * tileHeight;

        // Initialize source rectangle on the texture sheet
        SDL::FillRect(rectSource, xSource, ySource, tileWidth, tileHeight);

        // Calculate x and y position to render on screen
        int xDest = col * tileWidth;
        int yDest = row * tileHeight;

        // Initialize the destination rectangle for rendering
        SDL::FillRect(rectPlacement, xDest, yDest, tileWidth, tileHeight);

        // Copy the tile to the renderer
        SDL_RenderCopy(SDL::renderer, texture, &rectSource, &rectPlacement);
      }     // End frame rendering
    }   // End column rendering
  } // End row rendering  
}

// Print out info on the background to the screen
std::ostream& operator<<(std::ostream& out, const Background& background) {
  out << "Background:\n"
    << "Path: \'" << background.PATH << "\'\n"
    << "Texture Ptr: " << background.texture << '\n'
    << "Rectangle (x,y,w,h): " << static_cast<int>(background.rect.x) << ", " << static_cast<int>(background.rect.y) << ", " << static_cast<int>(background.rect.w) << ", " << static_cast<int>(background.rect.h) << '\n'
    << "Scrolling: (y + " << background.yOffset << ") @ " << background.scrollSpeed << " per frame";

  return out;
}
