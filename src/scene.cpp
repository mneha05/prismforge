#include "prismforge/scene.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace prismforge {

Scene defaultScene() {
  Scene scene;
  scene.spheres = {
      {{-1.35, 0.0, -0.5}, 1.0, {{0.12, 0.70, 0.92}, 0.25}},
      {{1.15, -0.20, -1.15}, 0.80, {{0.96, 0.28, 0.42}, 0.42}},
      {{0.15, 1.10, -2.25}, 0.55, {{0.44, 0.96, 0.72}, 0.18}},
  };
  scene.planes = {{{0.0, 1.0, 0.0}, 1.05, {{0.16, 0.18, 0.25}, 0.14}}};
  scene.lights = {
      {{-3.5, 5.5, 3.0}, {0.68, 0.84, 1.0}, 1.55},
      {{4.0, 2.8, 1.5}, {1.0, 0.38, 0.48}, 0.75},
  };
  return scene;
}

Scene loadScene(const std::string &path) {
  if (path.empty()) return defaultScene();
  std::ifstream input(path);
  if (!input) throw std::runtime_error("cannot open scene: " + path);
  Scene scene;
  std::string line;
  int line_number = 0;
  while (std::getline(input, line)) {
    ++line_number;
    const auto comment = line.find('#');
    if (comment != std::string::npos) line.erase(comment);
    std::istringstream parser(line);
    std::string kind;
    if (!(parser >> kind)) continue;
    if (kind == "camera") {
      parser >> scene.camera.position.x >> scene.camera.position.y >> scene.camera.position.z
             >> scene.camera.target.x >> scene.camera.target.y >> scene.camera.target.z
             >> scene.camera.vertical_fov_degrees;
    } else if (kind == "background") {
      parser >> scene.background.x >> scene.background.y >> scene.background.z;
    } else if (kind == "light") {
      Light light;
      parser >> light.position.x >> light.position.y >> light.position.z
             >> light.color.x >> light.color.y >> light.color.z >> light.intensity;
      scene.lights.push_back(light);
    } else if (kind == "sphere") {
      Sphere sphere;
      parser >> sphere.center.x >> sphere.center.y >> sphere.center.z >> sphere.radius
             >> sphere.material.color.x >> sphere.material.color.y >> sphere.material.color.z
             >> sphere.material.reflectivity;
      scene.spheres.push_back(sphere);
    } else if (kind == "plane") {
      Plane plane;
      parser >> plane.normal.x >> plane.normal.y >> plane.normal.z >> plane.distance
             >> plane.material.color.x >> plane.material.color.y >> plane.material.color.z
             >> plane.material.reflectivity;
      plane.normal = normalized(plane.normal);
      scene.planes.push_back(plane);
    } else {
      throw std::runtime_error("unknown scene command at line " + std::to_string(line_number));
    }
    if (!parser) throw std::runtime_error("invalid scene data at line " + std::to_string(line_number));
  }
  if (scene.lights.empty()) scene.lights.push_back({{-3.0, 5.0, 3.0}, {1.0, 1.0, 1.0}, 1.4});
  return scene;
}

}  // namespace prismforge
