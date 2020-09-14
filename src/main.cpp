#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
#include <vector>

#include "./geometry.hpp"
#include <fmt/core.h>

using namespace std;
using namespace fmt;

constexpr auto to_rad(float x) -> float { return (x / 180.f) * M_PI; }

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;
constexpr float    fov    = to_rad(90);

/*
 * Casts the ray onto objects returning the apropriate color
 */

auto cast_ray(const ray&            r,
              const vector<sphere>& spheres,
              const vector<light>&  lights = {}) -> vec3f
{
  auto ret  = vec3f { 0.2, 0.7, 0.8 };
  auto dist = std::numeric_limits<float>::max();

  for (const auto& s : spheres) {
    // on a hit
    if (auto d = s.ray_intersection(r)) {
      if (*d < dist) {
        auto light_intensity = 0.f;
        dist                 = *d;

        for (const auto& l : lights) {
          const auto hit   = r.origin + r.dir * dist;
          const auto normv = normalize(hit - s.center);
          const auto ldir  = normalize(l.position - hit);

          light_intensity += l.intensity * max(0.f, dot(ldir, normv));
        }

        ret = s.color * light_intensity;
      }
    }
  }

  return ret;
}

/*
 * Render all the objects
 */

auto render(const vector<sphere>& spheres, const vector<light>& lights) -> void
{
  constexpr auto fwidth  = static_cast<float>(width);
  constexpr auto fheight = static_cast<float>(height);

  const auto ang    = tanf(fov / 2.f);
  const auto origin = vec3f { 0, 0, 0 };

  auto buffer = vector<vec3f>(width * height);

#pragma omp parallel for
  for (size_t i = 0; i < height; i++) {
    for (size_t j = 0; j < width; j++) {
      const auto x = (2 * (j + .5f) / fwidth - 1) * ang * fwidth / fheight;
      const auto y = -(2 * (i + .5f) / fheight - 1) * ang;
      // Objects are projected onto a plane z = -1
      auto r = ray { origin, normalize(vec3f { x, y, -1 }) };

      buffer[j + (i * width)] = cast_ray(r, spheres, lights);
    }
  }

  // output as a ppm file
  auto ofs = ofstream("./out.ppm", ios::trunc);

  ofs << fmt::format("P6\n{} {}\n255\n", width, height);
  for (const auto& vec : buffer)
    for (const auto& val : vec)
      ofs << static_cast<char>(255 * max(0.0f, min(1.0f, val)));
}

auto main() -> int
{
  const auto ivory      = vec3f { 0.4, 0.4, 0.3 };
  const auto red_rubber = vec3f { 0.3, 0.1, 0.1 };
  const auto lights     = vector<light> { { { -20, 20, 20 }, 1.5 } };
  const auto spheres    = vector<sphere> {
    { { -3, 0, -16 }, 2, ivory },
    { { -1, -1.5, -12 }, 2, red_rubber },
    { { 1.5, -0.5, -18 }, 3, red_rubber },
    { { 7, 5, -18 }, 4, ivory },
  };

  render(spheres, lights);
}
