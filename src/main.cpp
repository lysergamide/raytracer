#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

#include "./geometry.hpp"
#include <fmt/core.h>

#define TO_RAD(X) ((X) / 180.0f) * M_PI

using namespace std;
using namespace fmt;

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;
constexpr float    fov    = TO_RAD(90);

/*
 * Casts the ray onto objects returning the apropriate color
 */

auto
cast_ray(const ray r, const vector<sphere> &spheres) -> vec3f
{
  auto ret       = vec3f { 0.2, 0.7, 0.8 };
  auto dist      = std::numeric_limits<float>::max();

  for (const auto &s : spheres) {
    if (auto d = s.ray_intersection(r)) {
      if (d < dist) {
        dist = *d;
        ret  = s.color;
      }
    }
  }

  return ret;
}

/*
 * Render all the objects
 */

auto
render(const vector<sphere> &spheres) -> void
{
  constexpr float fwidth  = width;
  constexpr float fheight = height;

  const auto ang    = tanf(fov / 2.0f);
  const auto origin = vec3f { 0, 0, 0 };

  auto buffer = vector<vec3f>(width * height);

#pragma omp parallel for
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      float x = (2.0f * (j + 0.5f) / fwidth - 1.f) * ang * fwidth / fheight;
      float y = -(2.0f * (i + 0.5f) / fheight - 1.f) * ang;
      // Objects are projected onto a plane z = -1
      auto r = ray(origin, normalize(vec3f { x, y, -1 }));

      buffer[j + (i * width)] = cast_ray(r, spheres);
    }
  }

  // output as a ppm file
  auto ofs = ofstream("./out.ppm", ios::trunc);
  ofs << fmt::format("P6\n{} {}\n255\n", width, height);

  for (const auto &vec : buffer)
    for (const auto &val : vec)
      ofs << static_cast<char>(255 * max(0.0f, min(1.0f, val)));
}

auto
main() -> int
{
  const auto ivory      = vec3f { 0.4, 0.4, 0.3 };
  const auto red_rubber = vec3f { 0.3, 0.1, 0.1 };

  auto spheres = vector<sphere> {
    sphere({ 0, 0, -16 }, 2, ivory),
    sphere({ 10, 0, -16 }, 1, red_rubber),
    sphere({ 0, -10, -16 }, 1, red_rubber),
    sphere({ 0, 2, -10 }, 1, red_rubber),
  };

  render(spheres);
}
