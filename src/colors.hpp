#pragma once

class ColorRGB {
public:
  float red;
  float green;
  float blue;

  ColorRGB() : red(0.0), green(0.0), blue(0.0) {}

  ColorRGB(float red, float green, float blue) :
    red(red), green(green), blue(blue) {}
};
