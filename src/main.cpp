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

constexpr unsigned width  = 1366;
constexpr unsigned height = 768;
constexpr float    fov    = to_rad(90);

/**
 * @brief: Casts a ray onto our scene
 *
 * @param r: A ray from the origin to a point on the scene
 * @param spheres: vector of spheres in the scene
 * @param lights: vector lights in the scene
 * @return vec3f represents the color of the point r intersects
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
  if (!sphr) { return vec3f { 0.2, 0.7, 0.8 }; }

  /**
   * phong reflection if there is a hit
   */

  const auto& mat   = sphr->mat;
  const auto  hit   = r.origin + r.dir * dist;  // point on sphere
  const auto  normv = normalize(hit - sphr->center);

  auto diff = 0.f;
  auto spec = 0.f;

  for (const auto& l : lights) {
    // direction from hit to light
    const auto l_dir = normalize(l.position - hit);
    // direction of reflected light
    const auto reflection = reflect(l_dir, normv);
    // distance between hit and light
    const auto hit_dist = mag(l.position - hit) - 0.001;
    // ray from light pointing to hit
    const auto l_ray = ray { l.position, -l_dir };

    // helper function, returns true if light is blocked
    const auto blocks = [&](const sphere& s) -> bool {
      auto s_dist = s.ray_intersection(l_ray).value_or(
        numeric_limits<float>::max());
      return hit_dist > s_dist;
    };
    // skip current light if it's blocked by another object
    if (ranges::any_of(spheres, blocks)) { continue; }

    diff += l.intensity * max(0.f, dot(l_dir, normv));
    spec += powf(max(0.f, dot(reflection, r.dir)), mat.spec_exp) * l.intensity;
  }

  return mat.color * (diff * mat.idiff + spec * mat.ispec);
}

auto render(const vector<sphere>& spheres, const vector<light>& lights) -> void
{
  constexpr auto fwidth  = static_cast<float>(width);
  constexpr auto fheight = static_cast<float>(height);

  const auto ang    = tanf(fov / 2.f);
  const auto origin = vec3f { 0, 0, 0 };
  // sphere's material
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

  // will overwrites existing file
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
