#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include "types.h"

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
    Direction movementDir;
    int SPEED = 1;

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
  
    friend std::ostream& operator<<(std::ostream& out, const AnimatedSprite& sprite);

  protected:
    AnimatedSprite(std::string filePath, int frames, int frameDelay);    // Helper constructor for child classes, ensure static member initialization
  
  public:
    int getWidth() const { return width; }    // Get the sprite width
    int getHeight() const { return height; }  // Get the sprite height
    void nextFrame(); // Advance sprite to next frame on sheet
    void draw();  // Draw sprite to render
    void setLocation(const Point2d& location);
    Point2d getLocation() const { return position; }
    void setDirection(const Direction& direction);
    void setSpeed(const int speed);
    bool move();
    void update();
};

bool checkCollision(const AnimatedSprite& sprite1, const AnimatedSprite& sprite2);

// Alien object
// Derives from AnimatedSprite parent

// Static variables for alien texture
extern SDL_Texture* alienTextureSheet;      // Texture sheet to share for all alien objects
extern std::string alienSheetPath;
extern std::string alienTransparency;

class Alien : public AnimatedSprite {
  private:
    //Enum class to hold color values
    enum class Color : int {  // Integral type - value corresponds with y-offset on the sheet
      blue,      // 0
      brown,       // 1
      gray,       // 2
      green,       // 3
      orange,      // 4
      pink,     // 5
      purple,      // 6
      red,        // 7
      yellow,     // 8
      MAX_COLORS  // Size - allows us to work with the sheet easier
    };

    Color color;
  
  public:
    Alien();
    ~Alien() = default;

    friend std::ostream& operator<<(std::ostream& out, const Alien& alien);

  public:
    static bool init();
    void moveDown();
    Color getColor() { return color; }
    friend class AlienRow;  // Allow AlienRow to access private and protected memebers

};

class AlienRow {
  public:
    static const int GAP_SIZE = 20;

  private:
    static const int SIZE = 10;
    static const int GUTTER_SIZE = 210;        // Width of side gutters
    Alien aliens[10];
    Rank RANK;

    bool empty = false;
    int yCollision;
    Direction xDir;

  public:
    AlienRow(Rank position);
    ~AlienRow() = default;

    friend std::ostream& operator<<(std::ostream& out, const AlienRow& row);

  public:
    void resetLocation();
    void moveDown();
    void update();
    void draw();
    bool isEmpty() { return empty; }

    friend class Bullets;   // Allow bullets to access members so we can determine collisions

};

// Static variables for bullet texture
extern SDL_Texture* bulletTextureSheet;
extern std::string bulletSheetPath;
extern std::string bulletTransparency;
extern int bulletCounter; // Holds the index of the next bullet to fire
extern int bulletTimer;   // Timer for all objects to wait before firing another bullet

class Bullet : public AnimatedSprite {
  public:
  private:
    bool active;  // Is the current object in an active state??
  public:
    Bullet(); // Construct an object
    ~Bullet() = default;

    friend std::ostream& operator<<(std::ostream& out, const Bullet& bullet);

  public: 
    static bool init();
    bool isActive() { return active; }
    void moveUp();
    void shoot();
    // Fire/Shoot object
    // Public "shoot"
    friend class Bullets; // Allow Bullets to access private and protected members
};

struct Bullets {
  public:
    static const int MAX_ACTIVE = 5;  // Maximum number of bullet objects active at a single time
    static const int BULLET_SPEED = 10;
  private:
    Bullet armory[MAX_ACTIVE];  // Store in array to make easier to work with
  public:
    Bullets();
    ~Bullets() = default;
    void update();
    void draw();
    void fire(const AnimatedSprite& player);
    bool checkCollisions(AlienRow& topRow, AlienRow& upperRow, AlienRow& lowerRow, AlienRow& bottomRow);

};

#endif
