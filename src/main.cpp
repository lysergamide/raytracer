#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>

#include "./geometry.hpp"
#include <fmt/core.h>

#define TO_RAD(X) ((X) / 180.0f) * M_PI

using namespace std;
using namespace fmt;

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;
constexpr float    fov    = TO_RAD(70);

auto
cast_ray(const vec3f ray, const vector<sphere> &spheres) -> vec3f
{
  //  if (!s.ray_intersection(ray)) return { 0.2, 0.7, 0.8 };
  for (auto &s : spheres) {
    if (s.ray_intersection(ray)) return s.color;
  }
  return { 0.2, 0.7, 0.8 };
}

auto
render(const vector<sphere> &spheres) -> void
{
  constexpr float fwidth  = width;
  constexpr float fheight = height;
  const float     ang     = tan(fov / 2.0f);

  auto fbuffer = vector<vec3f>(width * height);

#pragma omp parallel for
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      float y = (2.f * (i + 0.5f) / fheight - 1.f) * ang;
      float x = (2.f * (j + 0.5f) / fwidth - 1.f) * ang * fwidth / fheight;

      fbuffer[j + (i * width)] = cast_ray({ x, y, -1 }, spheres);
    }
  }

  auto ofs = ofstream("./out.ppm", ios::trunc);
  ofs << fmt::format("P6\n{} {}\n255\n", width, height);

  for (const auto &vec : fbuffer)
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
    sphere({ 10, 0, -16 }, 1),
    sphere({ 0, -10, -16 }, 1),
    sphere({ 0, 0, -5 }, 1, red_rubber),
  };


  render(spheres);
}
