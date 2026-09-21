#include <cassert>
#include <cmath>
#include <iostream>

#include "prismforge/math.hpp"
#include "prismforge/scene.hpp"
#include "prismforge/tracer.hpp"

int main() {
  using namespace prismforge;
  assert(std::abs(dot({1,2,3}, {4,5,6}) - 32.0) < 1e-12);
  const Vec3 unit = normalized({3,0,4});
  assert(std::abs(length(unit) - 1.0) < 1e-12);
  const Vec3 bounced = reflect({0,-1,0}, {0,1,0});
  assert(bounced.y > 0.999);
  Image image(8, 6);
  RenderConfig config{8, 6, 1, 1};
  renderRows(defaultScene(), config, image, 0, 1);
  double energy = 0.0;
  for (const Vec3 &pixel : image.pixels()) energy += pixel.x + pixel.y + pixel.z;
  assert(energy > 0.1);
  std::cout << "core tests passed; image energy=" << energy << '\n';
}
