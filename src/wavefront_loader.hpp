#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stdint.h>
#include <memory>

#include "models.hpp"
#include "geom.hpp"
#include "fileparsers/tga.hpp"

#define ALLOW_WAVEFRONT_FILE_PARSING_DEBUG_LOGS false
#define ALLOW_WAVEFRONT_FACES_PARSING_DEBUG_LOGS false
#define ALLOW_WAVEFRONT_LOADING_TEXTURE_DEBUG_LOGS false

class WavefrontObjLoader {
  typedef struct {
    std::vector<int> vertices;
    std::vector<int> textureVertices;
    std::vector<int> normalVertices;
  } rawFace_t;
 public:
  static Object3D loadObjWavefrontObj(const std::string filename, const std::string texturePath) {
    Object3D model = WavefrontObjLoader::loadObjWavefrontObj(filename);
    model.texture = WavefrontObjLoader::loadTexture(texturePath);
    return model;
  }

  static Object3D loadObjWavefrontObj(const std::string filename) {
    Object3D model;

    std::vector<dVector3D> vertices;
    std::vector<dVector3D> textureVectices;
    std::vector<dVector3D> normalVectices;
    std::vector<rawFace_t> faces;

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
      bool isTexture = line.compare(0, 3, "vt ") == 0;
      bool isNormal = line.compare(0, 3, "vn ") == 0;
      bool isFace = line.compare(0, 2, "f ") == 0;

      if (is3DPoint) {
        dVector3D point = WavefrontObjLoader::parseVectorLine(line);
        vertices.push_back(point);
#if ALLOW_WAVEFRONT_FILE_PARSING_DEBUG_LOGS
        std::cout << "v " << point.x() << " " << point.y() << " " << point.z()
                  << std::endl;
#endif
      } else if (isTexture) {
        dVector3D point = WavefrontObjLoader::parseVectorLine(line);
        textureVectices.push_back(point);
#if ALLOW_WAVEFRONT_FILE_PARSING_DEBUG_LOGS
        std::cout << "vt " << point.x() << " " << point.y() << " " << point.z()
                  << std::endl;
#endif
      } else if (isNormal) {
        dVector3D point = WavefrontObjLoader::parseVectorLine(line);
        normalVectices.push_back(point);
#if ALLOW_WAVEFRONT_FILE_PARSING_DEBUG_LOGS
        std::cout << "vt " << point.x() << " " << point.y() << " " << point.z()
                  << std::endl;
#endif
      } else if (isFace) {
        rawFace_t face = WavefrontObjLoader::parseFace(line);
        faces.push_back(face);
#if ALLOW_WAVEFRONT_FILE_PARSING_DEBUG_LOGS
        std::cout << "f " << face.vertices[0] << " " << face.vertices[1] << " " << face.vertices[2]
                  << " t " << face.textureVertices[0] << " " << face.textureVertices[1] << " " << face.textureVertices[2]
                  << " n " << face.normalVertices[0] << " " << face.normalVertices[1] << " " << face.normalVertices[2]
                  << std::endl;
#endif
      }
    }


    model.faces.reserve(faces.size());
    for (size_t i = 0; i < faces.size(); i++) {
      ObjectFace3D face;
      face.p0 = vertices[faces[i].vertices[0]];
      face.p1 = vertices[faces[i].vertices[1]];
      face.p2 = vertices[faces[i].vertices[2]];

      face.t0 = textureVectices[faces[i].textureVertices[0]];
      face.t1 = textureVectices[faces[i].textureVertices[1]];
      face.t2 = textureVectices[faces[i].textureVertices[2]];

      face.p0n = normalVectices[faces[i].normalVertices[0]];
      face.p1n = normalVectices[faces[i].normalVertices[1]];
      face.p2n = normalVectices[faces[i].normalVertices[2]];
      model.faces.push_back(face);

#if ALLOW_WAVEFRONT_FACES_PARSING_DEBUG_LOGS
      std::cout << "parsed face " << i << ": " << face.t0 << ", " << face.t1 << ", " << face.t2 << std::endl;
#endif
    }

#if ALLOW_WAVEFRONT_FILE_PARSING_DEBUG_LOGS
    std::cout << "total faces: " << model.faces.size() << std::endl;
#endif

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

  static rawFace_t parseFace(const std::string& line) {
    const char* s = line.c_str();
    rawFace_t face;
    face.vertices.reserve(3);
    face.textureVertices.reserve(3);
    face.normalVertices.reserve(3);

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
      face.vertices.push_back(vertex_index);

      const size_t c2 = f.find('/', c + 1);
      const std::string p2 = f.substr(c + 1, ((c2 - 1) - c));
      const int texture_vertex = std::atoi(p2.c_str()) - 1;
      face.textureVertices.push_back(texture_vertex);

      const size_t c3 = f.find('/', c2 + 1);
      const std::string p3 = f.substr(c2 + 1, ((c3 - 1) - c2));
      const int normalVertices = std::atoi(p3.c_str()) - 1;
      face.normalVertices.push_back(normalVertices);
    }

    return face;
  }

  static Texture2D loadTexture(const std::string& path) {
#if ALLOW_WAVEFRONT_LOADING_TEXTURE_DEBUG_LOGS
    std::cout << "loading texture... " << path.c_str() << std::endl;
#endif

    Tga file(path.c_str());
#if ALLOW_WAVEFRONT_LOADING_TEXTURE_DEBUG_LOGS
    std::cout << "texture loaded: " << file.GetWidth() << " x " << file.GetHeight() << " size " << file.GetPixels().size() << " bytes, alpha? " << file.HasAlphaChannel() << std::endl;
#endif
    return Texture2D(file.GetWidth(), file.GetHeight(), file.GetPixels());
  }
};
