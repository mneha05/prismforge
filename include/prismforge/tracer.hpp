#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "prismforge/scene.hpp"

namespace prismforge {

struct RenderConfig {
  int width{640};
  int height{360};
  int samples{1};
  int max_depth{2};
};

class Image {
 public:
  Image(int width, int height);
  Vec3 &at(int x, int y);
  const Vec3 &at(int x, int y) const;
  void writePpm(const std::string &path) const;
  int width() const { return width_; }
  int height() const { return height_; }
  std::vector<Vec3> &pixels() { return pixels_; }

 private:
  int width_;
  int height_;
  std::vector<Vec3> pixels_;
};

void renderRows(const Scene &scene, const RenderConfig &config, Image &image,
                int first_row, int row_stride);

}  // namespace prismforge
