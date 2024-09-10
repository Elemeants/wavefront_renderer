#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "geom.hpp"

#define ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS false

class ObjectFace3D {
public:
  dVector3D p0;
  dVector3D p1;
  dVector3D p2;

  dVector3D getSurfaceNormal() const { 
    return ~((p1 - p0) ^ (p2 - p0));
  }
};

class Object3D {
 public:
  std::vector<ObjectFace3D> faces;
};

class WavefrontObjLoader {
 public:
  static Object3D loadObjWavefrontObj(const std::string filename) {
    Object3D model;

    std::vector<dVector3D> vertices;
    std::vector<std::vector<int>> faces;

    std::ifstream in;
    std::cout << "Loading wavefront obj path: " << filename << std::endl;
    in.open(filename, std::ifstream::in);
    if (in.fail()) {
      std::cout << "Error loading wavefront object" << std::endl;
      exit(1);
    }

    std::string line;
    while (!in.eof()) {
      std::getline(in, line);
      bool is3DPoint = line.compare(0, 2, "v ") == 0;
      bool isFace = line.compare(0, 2, "f ") == 0;

      if (is3DPoint) {
        dVector3D point = WavefrontObjLoader::parseVectorLine(line);
        vertices.push_back(point);
#if ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS
        std::cout << "v " << point.x << " " << point.y << " " << point.z
                  << std::endl;
#endif
      } else if (isFace) {
        std::vector<int> face = WavefrontObjLoader::parseFace(line);
        faces.push_back(face);
#if ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS
        std::cout << "f " << face[0] << " " << face[1] << " " << face[2]
                  << std::endl;
#endif
      }
    }

#if ALLOW_WAVEFRONT_PARSING_DEBUG_LOGS
    std::cout << "total vertices: " << model.vertices.size()
              << ", total faces: " << model.faces.size() << std::endl;
#endif

    model.faces.reserve(faces.size());
    for (size_t i = 0; i < faces.size(); i++) {
      ObjectFace3D face;
      face.p0 = vertices[faces[i][0]];
      face.p1 = vertices[faces[i][1]];
      face.p2 = vertices[faces[i][2]];
      model.faces.push_back(face);
    }

    return model;
  }

 private:
  static dVector3D parseVectorLine(const std::string& line) {
    dVector3D point;
    std::stringstream ss;
    ss.str(line.c_str());
    std::string trash;

    ss >> trash >> point._Data[0] >> point._Data[1] >> point._Data[2];
    return point;
  }

  static std::vector<int> parseFace(const std::string& line) {
    const char* s = line.c_str();
    std::vector<int> vertices;
    vertices.reserve(3);

    std::stringstream ss;
    ss.str(s);

    std::string f;
    // Remove initial trash characters.
    ss >> f;
    for (size_t i = 0; i < 3; i++) {
      ss >> f;
      const size_t c = f.find('/');
      // Parse the index (starts with 0 so remove 1)
      const std::string p = f.substr(0, c);
      const int vertex_index = std::atoi(p.c_str()) - 1;
      vertices.push_back(vertex_index);
    }
    return vertices;
  }
};
