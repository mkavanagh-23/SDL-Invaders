#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL2/SDL.h>
#include <string>
#include <iostream>

// Background object
class Background {
  public:
    int scrollSpeed = 3;  // Speed at which BG scrolls in pixels
  private:
    SDL_Texture* texture = NULL; // Hold the background texture
    SDL_Rect rect;  // Render rectangle
    int yOffset = 0;  // Current y-offset for calculating scroll
    const std::string PATH; // Path to bitMap image

  public:
    Background(std::string filePath); // Constructor
    ~Background();  // Destructor

    // Override Operator<<
    friend std::ostream& operator<<(std::ostream& out, const Background& background);

  public:
    void scroll();  // Scroll the background by one tick
    void draw();  // Draw the background to the render
};

// Tilemap object
class Tilemap {
  private:
    SDL_Texture* texture = NULL;
    SDL_Rect rectSource;
    SDL_Rect rectPlacement;
    const std::string PATH = "";
    int* tiles = NULL;
    int sheetWidth = 0;
    int sheetHeight = 0;
    int mapCol = 0;
    int mapRow = 0;
    int tileWidth = 0;
    int tileHeight = 0;

  public:
    Tilemap(std::string filePath);
    ~Tilemap();

  public:
    void draw();
};
#endif
