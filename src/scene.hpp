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
        face.color0 = applyToSurfaceNormal(surfaceNormal);
      }
    }
  }

  void applyLightningToModelsSmooth() {
    for (Object3D& model : models) {
      for (ObjectFace3D& face : model.faces) {
        face.color0 = applyGouraud(face.getVertex0Normal());
        face.color1 = applyGouraud(face.getVertex1Normal());
        face.color2 = applyGouraud(face.getVertex2Normal());
      }
    }
  }

 private:
  ColorRGB applyGouraud(const dVector3D& surfaceNormal) const {
    const Light3D& light = lights[0];

    const double k_d = 0.005;
    // const double k_s = 0.0;
    const double k_a = 0.005;
    const double i_a = 0.09;

    const double ambient = k_a * i_a;
    const double diffuse = (k_d * 0.5 * (surfaceNormal % light.position));

    // const dVector3D lightgReflection = (surfaceNormal * ((light.position % surfaceNormal) * 2.0)) - light.position;
    // Doesn't work with current implementation, needs "fragment" shader to work.
    // const double specular = (k_s * pow(std::max(lightReflection % dVector3D(1, -1, 1), 0.0), 256));
    const double I_a = ambient + std::max(diffuse, 0.0);

    return light.color * std::min(I_a, 1.0);
  }

  ColorRGB applyToSurfaceNormal(const dVector3D& surfaceNormal) {
    std::vector<std::pair<float, Light3D>> lightsIntensity;
    lightsIntensity.resize(lights.size());

    std::transform(lights.begin(), lights.end(), lightsIntensity.begin(),
                   [&](const Light3D& light) -> std::pair<float, Light3D> {
                     const float i = surfaceNormal % light.position;
                     return std::make_pair(std::max(0.0f, i), light);
                   });

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
      lightsIntensity[i].first =
          (lightsIntensity[i].first / (magnitude * maxLightIntensity * 255.0f));
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
    return color;
  }
};
