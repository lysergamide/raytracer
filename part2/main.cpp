#include <fstream>
#include <iterator>
#include <vector>

#include "./geometry.hpp"
#include <fmt/core.h>

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;
constexpr float    fov    = M_PI / 2;

using namespace std;
using namespace fmt;

auto
cast_ray(const vec3f ray, const sphere &s) -> vec3f
{
  if (!s.ray_intersection(ray)) return { 0.2, 0.7, 0.8 };
  return { 0.4, 0.4, 0.3 };
}

auto
render(const sphere &s) -> void
{
  auto fbuffer = vector<vec3f>(width * height);

  constexpr float fwidth  = width;
  constexpr float fheight = height;
  const float     ang     = tan(fov / 2.0f);

#pragma omp parallel for
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      float y = (2.f * (i + 0.5f) / fheight - 1.f) * ang;
      float x = (2.f * (j + 0.5f) / fwidth - 1.f) * ang * width / fheight;

      fbuffer[j + (i * width)] = cast_ray(vec3f { x, y, -1 }, s);
    }
  }

  auto ofs = ofstream("./out.ppm", ios::trunc);
  ofs << fmt::format("P6\n{} {}\n255\n", width, height);

  for (const auto vec : fbuffer)
    for (const auto val : vec)
      ofs << static_cast<char>(255 * max(0.0f, min(1.0f, val)));
}

auto
main() -> int
{
  auto s = sphere(vec3f { -3, 0, -16 }, 2);
  render(s);
}
