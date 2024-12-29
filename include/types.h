#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <iostream>

// Struct to hold RGBA values
struct RGB {
  std::uint8_t r,g,b,a = 255;  // Amount of each color and alpha

  // Overide Operator<< to print the object
  friend std::ostream& operator<<(std::ostream& out, const RGB& color);
};  // End RGB

// Struct to hold (x, y) coordinates
struct Point2d {
  int x,y = 0;    // x,y-coordinate

  // Override Opertaor<<
  friend std::ostream& operator<<(std::ostream& out, const Point2d& point);
};

// Define directions for movement
// int(Can use as a speed multiplier)
enum class Direction : int {
  right = 1,        // X-Direction = 1
  left = -1         // X-Direction = -1
};

// Hold the rank for an alien row
// Rank can be used as an index
enum class Rank : int {
  first = 0,
  second = 1,
  third = 2,
  fourth = 3
};

// Convert a hex color value to an RGB object
const RGB hexToRGB(std::string hex);
std::ostream& operator<<(std::ostream& out, const Direction& direction);

#endif
