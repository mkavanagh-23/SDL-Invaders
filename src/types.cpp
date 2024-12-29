#include "types.h"
#include <cstdint>
#include <iostream>

/*** Override Operator<< to Print Objects to Console ***/
// Print RGB Colors
std::ostream& operator<<(std::ostream& out, const RGB& color) {
  out << "RGBA(" << static_cast<int>(color.r) << ", " << static_cast<int>(color.g) << ", " << static_cast<int>(color.b) << ", " << static_cast<int>(color.a) << ')';
  
  return out;
}

// Print X,Y Coordinate
std::ostream& operator<<(std::ostream& out, const Point2d& point) {
  out << "Point2d(" << point.x << ", " << point.y << ')';
  
  return out;
}

// Print direction values to the screen
std::ostream& operator<<(std::ostream& out, const Direction& direction) {
  switch(direction) {
    case Direction::left:
      out << "Left ";
      break;
    case Direction::right:
      out << "Right ";
      break;
    default:
      out << "UNDEF ";
      break;
  }
  out << '(' << static_cast<int>(direction) << ')';
  
  return out;
}

const RGB hexToRGB(const std::string hex) {
  std::string hexColor{ hex };  // Store the value in a temp string for processing
  if(hexColor[0] == '#') {    // If first character is '#', 
      hexColor = hexColor.substr(1, 6); // Extract the substring directly following it
  }

  return RGB{ // Convert each pair of chars into an RGB object
    static_cast<std::uint8_t>(std::stoi(hexColor.substr(0, 2), nullptr, 16)),
    static_cast<std::uint8_t>(std::stoi(hexColor.substr(2, 2), nullptr, 16)),
    static_cast<std::uint8_t>(std::stoi(hexColor.substr(4, 2), nullptr, 16))
  };
}
