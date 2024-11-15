#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <iostream>
#include <string>

// TODO:
// Bind enemy speed to background scroll speed and increment with each round
// The increased scroll speed gives a more stressful feel

/****************************** GLOBAL DATA ***********************************/

// Global Game Variables
SDL_Window* gameWindow = NULL;
SDL_Renderer* renderer = NULL;
SDL_Surface* tempSurface = NULL;

// Static alien texture
SDL_Texture* alienTextureSheet = NULL;      // Texture sheet to share for all alien objects
std::string alienSheetPath = "test/ufos.bmp";
std::string alienTransparency = "#00FF00";

// Global Game Constants
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1200;
const int GUTTER_SIZE = 210;        // Width of side gutters

// Global Game Variables
bool static_init = false;  // Track static class variable initialization state to be used as an invariant
int playerScore = 0;
int playerLives = 3;

// Interfaces
// Struct to hold RGBA values
struct RGB {
  Uint8 r = 255;  // Amount of red
  Uint8 g = 255;  // Amount of green
  Uint8 b = 255;  // Amount of blue
  Uint8 a = 255;    // Alpha transparency (default opaque)
};  // End RGB

// Struct to hold (x, y) coordinates
struct Point2d {
  int x = 0;    // x-coordinate
  int y = 0;    // y-coordinate
};

// Define directions for movement
// int(Can use as a speed multiplier)
enum class Direction : int {
  right = 1,        // X-Direction = 1
  left = -1         // X-Direction = -1
};

// Background object
class Background {
  private:
    SDL_Texture* texture = NULL; // Hold the background texture
    SDL_Rect rect;  // Render rectangle
    int yOffset = 0;  // Current y-offset for calculating scroll
    int scrollSpeed = 3;  // Speed at which BG scrolls in pixels
    const std::string PATH; // Path to bitMap image

  public:
    Background(std::string filePath); // Constructor
    ~Background();  // Destructor

  public:
    void scroll();  // Scroll the background by one tick
    void speedUp(int speedIncrease);    // Increase the background scroll speed
    void draw();  // Draw the background to the render
};

// Animated sprite object
class AnimatedSprite {
  protected:
    SDL_Texture* textureSheet = NULL; // Texture for entire sheet

  protected:
    //Render variables
    const std::string PATH = ""; // Path to bitMap texture file
    SDL_Rect rectPlacement; // Where to render the sprite on screen
    SDL_Rect rectSheet;   //  Rectangle to hold the entire sheet    //8 BYTES
    SDL_Rect rectSource;  // Rectangle to hold the current frame for placement

    //Animation variables
    int spriteFrame = 0;  // Current animation frame
    int frameCounter = 0; // Hold the count to delay frame rendering
    const int MAX_SPRITE_FRAME = 1; // Number of animation frames
    const int FRAME_DELAY = 5;  // How many frames to delay rendering

    //Attribute variables
    int width = 0;    // Width of a single sprite
    int height = 0;   // Height of a single sprite
    Point2d position = { 0, 0 };  // Current sprite location
    RGB transparency; // Sprite transparency color
    bool isDestroyed = false; // Sprite destruction state
    bool isExploded = false;  // Sprite explosion state 

  public:
    AnimatedSprite(std::string filePath, int frames, int frameDelay, const RGB& transparencyColor);
    AnimatedSprite(std::string filePath, int frames, int frameDelay, std::string transparencyHex);
    ~AnimatedSprite();
  
  protected:
    AnimatedSprite(std::string filePath, int frames, int frameDelay);    // Helper constructor for child classes, ensure static member initialization
  
  public:
    int getWidth() { return width; }    // Get the sprite width
    int getHeight() { return height; }  // Get the sprite height
    void nextFrame(); // Advance sprite to next frame on sheet
    void draw();  // Draw sprite to render
    void setLocation(const Point2d& location);
};

// Alien object
// Derives from AnimatedSprite parent
class Alien : public AnimatedSprite {
  private:
    //Enum class to hold color values
    enum class Color : int {  // Integral type - value corresponds with y-offset on the sheet
      green,      // 0
      blue,       // 1
      gray,       // 2
      pink,       // 3
      mauve,      // 4
      yellow,     // 5
      brown,      // 6
      red,        // 7
      orange,     // 8
      MAX_COLORS  // Size - allows us to work with the sheet easier
    };

    Color color;
  
  public:
    Alien(std::string filePath, int frames, int frameDelay, const RGB& transparencyColor);
    Alien(std::string filePath, int frames, int frameDelay, std::string transparencyHex);
    ~Alien() = default;

  public:

};

bool ProgramIsRunning();
void FillRect(SDL_Rect &rect, int x, int y, int width, int height);
SDL_Surface* loadImage(std::string path);
SDL_Texture* loadTexture(SDL_Surface* surface);
SDL_Surface* setTransparentColor (SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b);
void CloseShop();
bool Init();
const RGB hexToRGB(std::string_view hex);

/************************* MAIN GAME FUNCTION ***************************/
int main() {
  
  if(!Init()) {
    std::cout << "Critical error, terminating progra\nm";
    return 1;
  }


  //Create game objects
  Background background("test/bg1080.bmp");
  AnimatedSprite sprite("test/sprite.bmp", 16, 3, "#00FF00");
  Alien alien("test/ufos.bmp", 2, std::rand() % 100, "#00FF00");

  while(ProgramIsRunning())
  {
    sprite.nextFrame();
    alien.nextFrame();
    background.scroll();
    SDL_RenderClear(renderer);
    background.draw();
    sprite.draw();
    alien.draw();
    SDL_RenderPresent(renderer);
    SDL_Delay(20);
  }
  CloseShop();
  return 0;
}

/*********************** CLASS MEMBER FUNCTIONS *************************/
/*** Background Functions ***/
// Constructor
Background::Background(std::string filePath)
  : PATH{ filePath }
{
  tempSurface = loadImage(PATH);    //load the image
  texture = loadTexture(tempSurface);   //Load the image into a texture
  FillRect(rect, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);    //Create a rendering rectangle
}

// Destructor
Background::~Background() {
  SDL_DestroyTexture(texture);  // Destroy the texture object
}

// Increment BG by scrollSpeed
void Background::scroll() {
  yOffset += scrollSpeed;   //Increment the y-offset by the scroll speed
  if(yOffset >= SCREEN_HEIGHT)  //If the image has moved off the screen
    yOffset = 0;    //Reset the position
  rect.y = yOffset; //And store the new value in the render rectangle
}

void Background::speedUp(int speedIncrease) {
  scrollSpeed += speedIncrease;
}

// Draw the background to the render
void Background::draw() {
  SDL_RenderCopy(renderer, texture, NULL, &rect); // Copy the image to the render
  rect.y = yOffset - SCREEN_HEIGHT; // Scroll the image down
  SDL_RenderCopy(renderer, texture, NULL, &rect); //Copy the image to the render
}

/*** AnimatedSprite Functions ***/
// Constructors
AnimatedSprite::AnimatedSprite(std::string filePath, int frames, int frameDelay, const RGB& transparencyColor)
  : PATH{ filePath }, MAX_SPRITE_FRAME{ frames }, FRAME_DELAY{ frameDelay }, transparency{ transparencyColor }
{
  tempSurface = loadImage(PATH);  // Load the imahe
  tempSurface = setTransparentColor(tempSurface, transparency.r, transparency.g, transparency.b); // Set the surface transparency
  textureSheet = loadTexture(tempSurface); // Load the surface onto the texture
  SDL_QueryTexture(textureSheet, NULL, NULL, &rectSheet.w, &rectSheet.h); // Get the dimensions of the texture
  width = rectSheet.w / MAX_SPRITE_FRAME; // Calculate width of a single sprite
  height = rectSheet.h; // Height is equal to sheet height
  position.x = (SCREEN_WIDTH - width) / 2;  // Set initial starting position to bottom center of screen
  position.y = (SCREEN_HEIGHT - height) - 10;
  FillRect(rectSource, 0, 0, width, height);  // Create the source rectangle to render from
  FillRect(rectPlacement, position.x, position.y, width, height); // Create the destination render rectangle
}

AnimatedSprite::AnimatedSprite(std::string filePath, int frames, int frameDelay, std::string transparencyHex)
  : AnimatedSprite(filePath, frames, frameDelay, hexToRGB(transparencyHex))
{}   // Convert the hex value to an RGB object and call the constructor as normal

AnimatedSprite::AnimatedSprite(std::string filePath, int frames, int frameDelay)  // Protected helper constructor for alien sprites
  : PATH{ filePath }, MAX_SPRITE_FRAME{ frames }, FRAME_DELAY{ frameDelay }  // Sets const instance data members, the rest is handled by child constructor
{
  // Ensure static members have been initialized before constructing object
  assert((static_init == true) && "Fatal Error: Tried to create sprite object before initializing static members.");
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
  SDL_RenderCopy(renderer, textureSheet, &rectSource, &rectPlacement);
}

void AnimatedSprite::setLocation(const Point2d& location){
  position = location;  // Set position to the given location
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
  FillRect(rectSource, sourceX, sourceY, width, height);
  FillRect(rectPlacement, position.x, position.y, width, height);

  // DEBUG STATEMENT
  std::cout << "Generated alien with color #" << int(color) << '\n';
}

Alien::Alien(std::string filePath, int frames, int frameDelay, std::string transparencyHex)
  : Alien(filePath, frames, frameDelay, hexToRGB(transparencyHex))
{}

bool ProgramIsRunning() {
    SDL_Event event;

    bool running = true;

    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
            running = false;
    }

    return running;
}

void FillRect(SDL_Rect &rect, int x, int y, int width, int height) {
    //Initialize the rectangle
    rect.x = x;         //initial x position of upper left corner
    rect.y = y;         //initial y position of upper left corner
    rect.w = width;     //width of rectangle
    rect.h = height;    //height of rectangle
}//end rectangle initializing

SDL_Surface* loadImage(std::string path) {
    //Load image at specified path
    tempSurface = SDL_LoadBMP(path.c_str());

    if( tempSurface == NULL )
    {
        printf("Unable to load image at path: %s\n", path.c_str());
    }
    return tempSurface;
}//end loadImage

SDL_Texture* loadTexture(SDL_Surface* tempSurface) {
    //texture
    SDL_Texture* newTexture = NULL;
    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    if( newTexture == NULL )
    {
        printf("Unable to create texture\n");
    }
    //Get rid of the surface
    SDL_FreeSurface( tempSurface );

    return newTexture;
}//end create a texture

SDL_Surface* setTransparentColor (SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b) {
    //create the transparent color (pink in this case) and put in colorKey
    Uint32 colorKey;
    colorKey = SDL_MapRGB(surface->format, r, g, b);
    //set the colorKey color as the transparent one in the image
    SDL_SetColorKey(surface, SDL_TRUE, colorKey);
    return surface;
}//End setTransparent Color

void CloseShop() {
    //Destroy all objects
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    SDL_Quit(); //Quit the program
}//End Close Shop

bool Init() {
  // Initialize SDL
  // Create the renderer and the window
  gameWindow = SDL_CreateWindow("Final Project (DEBUG)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(gameWindow, -1, 0);
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0) { // Initialize all SDL objects
    std::cout << "Failed to initialize SDL!\n";
    return false;
  }

  std::srand(std::time(0)); // Seed the random number generator for object creation
  
  //Initialize the static alien texture
  RGB color = hexToRGB(alienTransparency);
  tempSurface = loadImage(alienSheetPath);
  tempSurface = setTransparentColor(tempSurface, color.r, color.g, color.b);
  alienTextureSheet = loadTexture(tempSurface);
  if(alienTextureSheet == NULL) {
    std::cout << "Failed to intitialize static textures!\n";
    return false;
  }

  static_init = true;  // Mark static initialization as completed
  return true;
}

const RGB hexToRGB(const std::string_view hex) {
  std::string hexColor{ hex };  // Store the value in a temp string for processing
  if(hexColor[0] == '#') {    // If first character is '#', 
      hexColor = hexColor.substr(1, 6); // Extract the substring directly following it
  }

  return RGB{ // Convert each pair of chars into an RGB object
    static_cast<Uint8>(std::stoi(hexColor.substr(0, 2), nullptr, 16)),
    static_cast<Uint8>(std::stoi(hexColor.substr(2, 2), nullptr, 16)),
    static_cast<Uint8>(std::stoi(hexColor.substr(4, 2), nullptr, 16))
  };
}
