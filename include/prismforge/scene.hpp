#pragma once

#include <string>
#include <vector>

#include "prismforge/math.hpp"

namespace prismforge {

struct Material {
  Vec3 color{0.7, 0.7, 0.7};
  double reflectivity{0.0};
};

struct Sphere { Vec3 center; double radius; Material material; };
struct Plane { Vec3 normal; double distance; Material material; };
struct Light { Vec3 position; Vec3 color; double intensity; };

struct Camera {
  Vec3 position{0.0, 1.3, 5.5};
  Vec3 target{0.0, 0.4, 0.0};
  double vertical_fov_degrees{48.0};
};

struct Scene {
  Camera camera;
  Vec3 background{0.015, 0.025, 0.06};
  std::vector<Sphere> spheres;
  std::vector<Plane> planes;
  std::vector<Light> lights;
};

Scene loadScene(const std::string &path);
Scene defaultScene();

}  // namespace prismforge
