#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

#include "prismforge/scene.hpp"
#include "prismforge/tracer.hpp"

namespace {
int parseInt(const char *value, const std::string &name) {
  const int parsed = std::stoi(value);
  if (parsed <= 0) throw std::invalid_argument(name + " must be positive");
  return parsed;
}
}

int main(int argc, char **argv) {
  prismforge::RenderConfig config;
  std::string scene_path;
  std::string output_path = "render.ppm";
  try {
    for (int i = 1; i < argc; ++i) {
      const std::string argument = argv[i];
      if (argument == "--width" && i + 1 < argc) config.width = parseInt(argv[++i], "width");
      else if (argument == "--height" && i + 1 < argc) config.height = parseInt(argv[++i], "height");
      else if (argument == "--samples" && i + 1 < argc) config.samples = parseInt(argv[++i], "samples");
      else if (argument == "--scene" && i + 1 < argc) scene_path = argv[++i];
      else if (argument == "--output" && i + 1 < argc) output_path = argv[++i];
      else throw std::invalid_argument("unknown or incomplete argument: " + argument);
    }
    const prismforge::Scene scene = prismforge::loadScene(scene_path);
    prismforge::Image image(config.width, config.height);
    const auto start = std::chrono::steady_clock::now();
    prismforge::renderRows(scene, config, image, 0, 1);
    const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    image.writePpm(output_path);
    std::cout << "rendered " << config.width << 'x' << config.height << " in " << seconds
              << " s -> " << output_path << '\n';
  } catch (const std::exception &error) {
    std::cerr << "error: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
