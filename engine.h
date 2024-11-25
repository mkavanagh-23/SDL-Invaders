#ifndef ENGINE_H
#define ENGINE_H

#include <SDL2/SDL.h>
#include <string>

namespace SDL {
  extern SDL_Window* gameWindow;
  extern SDL_Renderer* renderer;
  extern SDL_Surface* tempSurface;
  extern bool static_init;  // Track static class variable initialization state to be used as an invariant
  
  bool ProgramIsRunning();
  void FillRect(SDL_Rect &rect, int x, int y, int width, int height);
  SDL_Surface* loadImage(std::string path);
  SDL_Texture* loadTexture(SDL_Surface* surface);
  SDL_Surface* setTransparentColor (SDL_Surface* surface, Uint8 r, Uint8 g, Uint8 b);
  bool Init();
  void CloseShop();
}

#endif
