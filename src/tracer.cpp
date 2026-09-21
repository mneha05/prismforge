#include "prismforge/tracer.hpp"

#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace prismforge {
namespace {
constexpr double kEpsilon = 1e-4;
constexpr double kPi = 3.14159265358979323846;

struct Hit {
  bool found{false};
  double distance{std::numeric_limits<double>::infinity()};
  Vec3 point;
  Vec3 normal;
  Material material;
};

Hit closestHit(const Scene &scene, const Ray &ray) {
  Hit best;
  for (const Sphere &sphere : scene.spheres) {
    const Vec3 oc = ray.origin - sphere.center;
    const double half_b = dot(oc, ray.direction);
    const double c = dot(oc, oc) - sphere.radius * sphere.radius;
    const double discriminant = half_b * half_b - c;
    if (discriminant < 0.0) continue;
    const double root = std::sqrt(discriminant);
    double distance = -half_b - root;
    if (distance <= kEpsilon) distance = -half_b + root;
    if (distance <= kEpsilon || distance >= best.distance) continue;
    best.found = true;
    best.distance = distance;
    best.point = ray.origin + ray.direction * distance;
    best.normal = normalized(best.point - sphere.center);
    best.material = sphere.material;
  }
  for (const Plane &plane : scene.planes) {
    const double denominator = dot(plane.normal, ray.direction);
    if (std::abs(denominator) < 1e-8) continue;
    const double distance = -(dot(plane.normal, ray.origin) + plane.distance) / denominator;
    if (distance <= kEpsilon || distance >= best.distance) continue;
    best.found = true;
    best.distance = distance;
    best.point = ray.origin + ray.direction * distance;
    best.normal = denominator < 0.0 ? plane.normal : -plane.normal;
    best.material = plane.material;
  }
  return best;
}

Vec3 trace(const Scene &scene, const Ray &ray, int depth) {
  const Hit hit = closestHit(scene, ray);
  if (!hit.found) {
    const double blend = 0.5 * (ray.direction.y + 1.0);
    return scene.background * (1.0 - blend) + Vec3{0.05, 0.11, 0.20} * blend;
  }

  Vec3 color = hit.material.color * 0.055;
  for (const Light &light : scene.lights) {
    const Vec3 offset = light.position - hit.point;
    const double light_distance = length(offset);
    const Vec3 light_direction = offset / light_distance;
    const Ray shadow_ray{hit.point + hit.normal * kEpsilon, light_direction};
    const Hit shadow = closestHit(scene, shadow_ray);
    if (shadow.found && shadow.distance < light_distance) continue;
    const double diffuse = std::max(0.0, dot(hit.normal, light_direction));
    const double attenuation = light.intensity / (1.0 + 0.035 * light_distance * light_distance);
    color += hadamard(hit.material.color, light.color) * (diffuse * attenuation);
    const Vec3 half_vector = normalized(light_direction - ray.direction);
    color += light.color * (std::pow(std::max(0.0, dot(hit.normal, half_vector)), 48.0) * attenuation * 0.32);
  }
  if (depth > 0 && hit.material.reflectivity > 0.0) {
    const Ray reflected{hit.point + hit.normal * kEpsilon, normalized(reflect(ray.direction, hit.normal))};
    const Vec3 reflected_color = trace(scene, reflected, depth - 1);
    color = color * (1.0 - hit.material.reflectivity) + reflected_color * hit.material.reflectivity;
  }
  return clamp01(color);
}

Ray cameraRay(const Camera &camera, int x, int y, int width, int height,
              double jitter_x, double jitter_y) {
  const Vec3 forward = normalized(camera.target - camera.position);
  const Vec3 right = normalized(cross(forward, {0.0, 1.0, 0.0}));
  const Vec3 up = cross(right, forward);
  const double aspect = static_cast<double>(width) / height;
  const double scale = std::tan(camera.vertical_fov_degrees * kPi / 360.0);
  const double screen_x = (2.0 * ((x + jitter_x) / width) - 1.0) * aspect * scale;
  const double screen_y = (1.0 - 2.0 * ((y + jitter_y) / height)) * scale;
  return {camera.position, normalized(forward + right * screen_x + up * screen_y)};
}
}  // namespace

Image::Image(int width, int height)
    : width_(width), height_(height), pixels_(static_cast<std::size_t>(width * height)) {
  if (width <= 0 || height <= 0) throw std::invalid_argument("image dimensions must be positive");
}

Vec3 &Image::at(int x, int y) { return pixels_[static_cast<std::size_t>(y * width_ + x)]; }
const Vec3 &Image::at(int x, int y) const { return pixels_[static_cast<std::size_t>(y * width_ + x)]; }

void Image::writePpm(const std::string &path) const {
  std::ofstream output(path, std::ios::binary);
  if (!output) throw std::runtime_error("cannot write image: " + path);
  output << "P6\n" << width_ << ' ' << height_ << "\n255\n";
  for (const Vec3 &pixel : pixels_) {
    const Vec3 gamma{std::sqrt(std::clamp(pixel.x, 0.0, 1.0)),
                     std::sqrt(std::clamp(pixel.y, 0.0, 1.0)),
                     std::sqrt(std::clamp(pixel.z, 0.0, 1.0))};
    const unsigned char rgb[3] = {
        static_cast<unsigned char>(255.0 * gamma.x),
        static_cast<unsigned char>(255.0 * gamma.y),
        static_cast<unsigned char>(255.0 * gamma.z)};
    output.write(reinterpret_cast<const char *>(rgb), 3);
  }
}

void renderRows(const Scene &scene, const RenderConfig &config, Image &image,
                int first_row, int row_stride) {
  for (int y = first_row; y < config.height; y += row_stride) {
    for (int x = 0; x < config.width; ++x) {
      Vec3 accumulated;
      for (int sample = 0; sample < config.samples; ++sample) {
        const double jitter_x = config.samples == 1 ? 0.5 : std::fmod(sample * 0.754877666, 1.0);
        const double jitter_y = config.samples == 1 ? 0.5 : std::fmod(sample * 0.569840296, 1.0);
        accumulated += trace(scene, cameraRay(scene.camera, x, y, config.width, config.height,
                                              jitter_x, jitter_y), config.max_depth);
      }
      image.at(x, y) = accumulated / static_cast<double>(config.samples);
    }
  }
}

}  // namespace prismforge
