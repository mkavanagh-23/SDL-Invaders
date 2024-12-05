#include "engine.h"
#include "settings.h"
#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <ctime>

namespace SDL {
  // Declare global SDL objects
  SDL_Window* gameWindow = NULL;
  SDL_Renderer* renderer = NULL;
  SDL_Surface* tempSurface = NULL;
  bool alien_init = false;
  bool bullet_init = false;
  bool static_init = false;         // Track static class variable initialization state to be used as an invariant

  
  // Check if SDL is running
  bool ProgramIsRunning() {
    SDL_Event event;
    bool running = true;

    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_QUIT)
        running = false;
    }

    return running;
  }

  SDL_Surface* loadImage(std::string path) {
    //Load image at specified path
    tempSurface = SDL_LoadBMP(path.c_str());

    if( tempSurface == NULL ) {
        printf("Unable to load image at path: %s\n", path.c_str());
    }

    return tempSurface;
  }//end loadImage

  SDL_Texture* loadTexture(SDL_Surface* tempSurface) {
    //texture
    SDL_Texture* newTexture = NULL;
    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    if( newTexture == NULL ) {
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

  void FillRect(SDL_Rect &rect, int x, int y, int width, int height) {
    //Initialize the rectangle
    rect.x = x;         //initial x position of upper left corner
    rect.y = y;         //initial y position of upper left corner
    rect.w = width;     //width of rectangle
    rect.h = height;    //height of rectangle
  }//end rectangle initializing

  bool Init() {
    // Initialize SDL
    // Create the renderer and the window
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0) { // Initialize all SDL objects
      std::cout << "Failed to initialize SDL!\n";
      return false;
    }

    gameWindow = SDL_CreateWindow("Matthew Kavanagh  |  CSC222-301W  |  Final Project  |  SDL Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, settings::SCREEN_WIDTH, settings::SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(gameWindow, -1, 0);
    
    std::srand(std::time(0)); // Seed the random number generator for object creation
    return true;
  }

  void CloseShop() {
    //Destroy all objects
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(gameWindow);
    tempSurface = NULL;
    gameWindow = NULL;
    renderer = NULL;
    SDL_Quit(); //Quit the program
  }//End Close Shop
}
