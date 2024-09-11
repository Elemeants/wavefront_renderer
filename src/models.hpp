#pragma once

#include <vector>

#include "colors.hpp"
#include "geom.hpp"

#include "GL/glu.h"

class ObjectFace3D {
 public:
  dVector3D p0;
  dVector3D p1;
  dVector3D p2;
  ColorRGB color;

  dVector3D t0;
  dVector3D t1;
  dVector3D t2;

  dVector3D getSurfaceNormal() const { return ~((p1 - p0) ^ (p2 - p0)); }
};

class Texture2D {
 public:
  size_t width;
  size_t height;
  std::shared_ptr<uint8_t*> data;
  GLuint textureRef;

  Texture2D() {
    this->data = std::make_shared<uint8_t*>();
  }

  Texture2D(size_t width, size_t height, std::vector<uint8_t> textureData)
      : width(width), height(height) {
    this->data = std::make_shared<uint8_t*>(new uint8_t[textureData.size()]);
    for (size_t i = 0; i < textureData.size(); ++i) {
      (*data)[i] = textureData[i];
    }
  }
};

class Object3D {
 public:
  std::vector<ObjectFace3D> faces;
  Texture2D texture;
};
