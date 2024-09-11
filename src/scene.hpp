#pragma once

#include <vector>

#include "lights.hpp"
#include "models.hpp"

class Scene {
 public:
  std::vector<Object3D> models;
  std::vector<Light3D> lights;

  void applyLightingToModels() {
    for (Object3D& model : models) {
      for (ObjectFace3D& face : model.faces) {
        const dVector3D surfaceNormal = face.getSurfaceNormal();

        std::vector<std::pair<float, Light3D>> lightsIntensity;
        lightsIntensity.resize(lights.size());

        std::transform(lights.begin(), lights.end(), lightsIntensity.begin(),
                       [&](const Light3D& light) -> std::pair<float, Light3D> {
                         const float i = surfaceNormal % light.position;
                         return std::make_pair(i, light);
                       });

        // Remove negative light intensity
        lightsIntensity.erase(
            std::remove_if(lightsIntensity.begin(), lightsIntensity.end(),
                           [](const std::pair<float, Light3D>& light) {
                             return light.first < 0;
                           }),
            lightsIntensity.end());

        // Normalize light intensity
        float magnitude = 0;
        float maxLightIntensity = 0;
        for (size_t i = 0; i < lightsIntensity.size(); ++i) {
          magnitude += lightsIntensity[i].first;
          if (lightsIntensity[i].first > maxLightIntensity) {
            maxLightIntensity = lightsIntensity[i].first;
          }
        }

        // After normalization get the color on `float`
        for (size_t i = 0; i < lightsIntensity.size(); ++i) {
          lightsIntensity[i].first = (lightsIntensity[i].first /
                                      (magnitude * maxLightIntensity * 255.0f));
          lightsIntensity[i].second.color.red *= lightsIntensity[i].first;
          lightsIntensity[i].second.color.green *= lightsIntensity[i].first;
          lightsIntensity[i].second.color.blue *= lightsIntensity[i].first;
        }

        // Mix the colors
        ColorRGB color = {0, 0, 0};
        for (size_t i = 0; i < lightsIntensity.size(); ++i) {
          color.red += lightsIntensity[i].second.color.red;
          color.green += lightsIntensity[i].second.color.green;
          color.blue += lightsIntensity[i].second.color.blue;
        }

        face.color = color;
      }
    }
  }
};
