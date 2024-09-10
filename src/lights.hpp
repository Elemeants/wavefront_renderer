#pragma once

#include "geom.hpp"

class ColorRGB {
public:
  float red;
  float green;
  float blue;

  ColorRGB() : red(0.0), green(0.0), blue(0.0) {}

  ColorRGB(float red, float green, float blue) :
    red(red), green(green), blue(blue) {}
};

#define COLOR_WHITE ColorRGB(255, 255, 255)
#define COLOR_RED ColorRGB(255, 0, 0)
#define COLOR_GREEN ColorRGB(0, 255, 0)
#define COLOR_BLUE ColorRGB(0, 0, 255)

class Light3D {
public:
  ColorRGB color;
  dVector3D position;

  Light3D() : color(0.0, 0.0, 0.0), position(0.0, 0.0, 0.0) {}

  Light3D(const double red, const double green, const double blue, dVector3D position) :
    color(red, green, blue), position(position) {}

  Light3D(const ColorRGB color, dVector3D position) :
    color(color), position(position) {}
};
