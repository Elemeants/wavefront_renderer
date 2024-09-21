#pragma once

class ColorRGB {
 public:
  float red;
  float green;
  float blue;

  ColorRGB() : red(0.0), green(0.0), blue(0.0) {}

  ColorRGB(float red, float green, float blue)
      : red(red), green(green), blue(blue) {}
};

ColorRGB operator+(const ColorRGB& left, const ColorRGB& right) {
  ColorRGB result;
  result.red = left.red + right.red;
  result.green = left.green + right.green;
  result.blue = left.blue + right.blue;
  return result;
}

ColorRGB operator-(const ColorRGB& left, const ColorRGB& right) {
  ColorRGB result;
  result.red = left.red - right.red;
  result.green = left.green - right.green;
  result.blue = left.blue - right.blue;
  return result;
}

ColorRGB operator*(const ColorRGB& left, const ColorRGB& right) {
  ColorRGB result;
  result.red = left.red * right.red;
  result.green = left.green * right.green;
  result.blue = left.blue * right.blue;
  return result;
}

ColorRGB operator*(const ColorRGB& left, const double right) {
  ColorRGB result;
  result.red = left.red * right;
  result.green = left.green * right;
  result.blue = left.blue * right;
  return result;
}
