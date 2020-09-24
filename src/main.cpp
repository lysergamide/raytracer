#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>
#include <ranges>
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
  auto dist = numeric_limits<float>::max();
  auto sphr = optional<sphere> {};

  // find nearest sphere if it exists
  for (const auto& s : spheres) {
    if (auto d = s.ray_intersection(r)) {
      if (*d < dist) {
        dist = *d;
        sphr = s;
      }
    }
  }

  // default color if no hit
  if (!sphr) { return { 0.2, 0.7, 0.8 }; }

  // phong reflection if theres a hit
  auto diff = 0.f;
  auto spec = 0.f;

  const auto& color    = sphr->mat.color;
  const auto  ispec    = sphr->mat.ispec;
  const auto  idiff    = sphr->mat.idiff;
  const auto  spec_exp = sphr->mat.spec_exp;

  for (const auto& l : lights) {
    const auto hit        = r.origin + r.dir * dist;
    const auto normv      = normalize(hit - sphr->center);
    const auto ldir       = normalize(l.position - hit);
    const auto reflection = reflect(ldir, normv);

    const auto ldist = mag(l.position - hit);
    // skip current light if it's blocked by another object
    if (any_of(begin(spheres), end(spheres), [&](const auto& s) {
          auto shadow = normalize(hit - normv);
          return s.ray_intersection(
                    { l.position, normalize(shadow - l.position) })
                   .value_or(numeric_limits<float>::max())
                 < ldist;
        })) {
      continue;
    }

    // diffuse lighting
    diff += l.intensity * max(0.f, dot(ldir, normv));

    // specular lighting
    spec += powf(max(0.f, dot(reflection, r.dir)), spec_exp) * l.intensity;
  }

  return color * (diff * idiff + spec * ispec);
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

  // trunc overwrites existing file
  auto ofs = ofstream("./out.ppm", ios::trunc);

  ofs << fmt::format("P6\n{} {}\n255\n", width, height);
  for (const auto& vec : buffer)
    for (const auto& val : vec)
      ofs << static_cast<char>(255 * max(0.0f, min(1.0f, val)));
}

auto main() -> int
{
  const auto ivory      = material({ 0.4, 0.4, 0.3 }, 0.6, 0.3, 50);
  const auto red_rubber = material({ 0.3, 0.1, 0.1 }, 0.9, 0.1, 10);

  const auto lights = vector<light> { light({ -20, 20, 20 }, 1.5),
                                      light({ 30, 50, -26 }, 1.8),
                                      light({ 30, 20, 30 }, 1.7) };

  const auto spheres = vector<sphere> {
    sphere({ -3, 0, -16 }, 2, ivory),
    sphere({ -1, -1.5, -12 }, 2, red_rubber),
    sphere({ 1.5, -0.5, -18 }, 3, red_rubber),
    sphere({ 7, 5, -18 }, 4, ivory),
  };

  render(spheres, lights);
}
