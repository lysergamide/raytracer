#include <algorithm>
#include <fstream>
#include <limits>
#include <ranges>
#include <vector>

#include "./geometry.hpp"
#include <fmt/core.h>

using namespace std;
using namespace fmt;

// constexpr auto to_rad(float x) -> float { return (x / 180.f) * M_PI; }

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;
constexpr float    fov    = M_PI / 2;

/**
 * @brief: Casts a ray onto our scene
 *
 * @param r: A ray from the origin to a point on the scene
 * @param spheres: vector of spheres in the scene
 * @param lights: vector lights in the scene
 * @param depth: current stack depth
 * @return vec3f represents the color of the point r intersects
 */
auto cast_ray(const ray&            r,
              const vector<sphere>& spheres,
              const vector<light>&  lights,
              const size_t          depth = 0) -> vec3f
{
    constexpr auto default_color = vec3f{ 0.2, 0.7, 0.8 };
    // depth limit guard
    if (depth > 4) return default_color;

    auto dist = numeric_limits<float>::max();
    auto sphr = optional<sphere>{};

    // find nearest sphere if it exists
    for (const auto& s : spheres) {
        const auto d = s.ray_intersection(r);

        if (d && d.value() < dist) {
            dist = d.value();
            sphr = s;
        }
    }

    // not hit
    if (!sphr) return default_color;

    /********************
     * Phong reflection *
     ********************/

    // point on sphere
    const auto hit = r.origin + r.dir * dist;
    // direction from center of sphere to hit
    const auto normv = normalize(hit - sphr->center);

    /**
     * Mirror like reflection
     */

    const auto re_dir    = reflect(-r.dir, normv);
    const auto re_origin = dot(re_dir, normv) >= 0 ? hit + normv * 0.0001
                                                   : hit - normv * 0.0001;
    const auto re_ray = ray(re_origin, re_dir);
    const auto reff   = cast_ray(re_ray, spheres, lights, depth + 1)
                      * sphr->mat.albedo[2];
    /**
     * Specular and diffuse lighting
     */

    auto diff = 0.f;
    auto spec = 0.f;

    for (const auto& l : lights) {
        // direction from hit to light
        const auto l_dir = normalize(l.position - hit);
        // direction of reflected light
        const auto reflection = reflect(l_dir, normv);
        // distance between hit and light
        const auto hit_dist = magnitude(l.position - hit) - 0.1;
        // ray from light pointing to hit
        const auto l_ray = ray{ l.position, -l_dir };

        // helper function, returns true if light is blocked
        const auto blocks = [&](const sphere& s) -> bool {
            return abs(hit_dist) > abs(s.ray_intersection(l_ray).value_or(
                     numeric_limits<float>::max()));
        };
        // skip current light if it's blocked by another object
        if (ranges::any_of(spheres, blocks)) { continue; }

        diff += l.intensity * max(0.f, dot(l_dir, normv));
        spec += powf(max(0.f, dot(reflection, r.dir)), sphr->mat.spec_exp)
                * l.intensity;
    }
    diff *= sphr->mat.albedo[0];
    spec *= sphr->mat.albedo[1];

    return (sphr->mat.color * (diff + spec)) + reff;
}

auto render(const vector<sphere>& spheres, const vector<light>& lights) -> void
{
    constexpr auto fwidth  = static_cast<float>(width);
    constexpr auto fheight = static_cast<float>(height);

    const auto ang    = tanf(fov / 2.f);
    const auto origin = vec3f{ 0, 0, -2 };
    // sphere's material
    auto buffer = vector<vec3f>(width * height);

#pragma omp parallel for
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            const auto x = (2 * (j + .5f) / fwidth - 1) * ang * fwidth
                           / fheight;
            const auto y = -(2 * (i + .5f) / fheight - 1) * ang;
            // projected onto a plane z = -1
            auto r = ray{ origin, normalize(vec3f{ x, y, -1 }) };

            buffer[j + (i * width)] = cast_ray(r, spheres, lights);
        }
    }

    // will overwrite existing file
    auto ofs = ofstream("./out.ppm", ios::trunc);

    ofs << fmt::format("P6\n{} {}\n255\n", width, height);
    for (const auto& vec : buffer)
        for (const auto& val : vec)
            ofs << static_cast<char>(255 * max(0.0f, min(1.0f, val)));
}

auto main() -> int
{
    const auto ivory      = material({ 0.4, 0.4, 0.3 }, { 0.6, 0.3, 0.1 }, 50);
    const auto red_rubber = material({ 0.3, 0.1, 0.1 }, { 0.9, 0.1, 0 }, 10);
    const auto mirror     = material({ 1, 1, 1 }, { 0.01, 10, 0.9 }, 1425);
    const auto jade       = material({ 0.2, 0.6, 0.5 }, { 0.5, 0.8, .2 }, 100);

    const auto lights = vector<light>{
        light({ -20, 20, 20 }, 1.5),
        light({ 30, 50, -25 }, 1.8),
        light({ 30, 20, 30 }, 1.7),
    };

    const auto spheres = vector<sphere>{
        sphere({ -3, 0, -16 }, 2, ivory),
        sphere({ -1, -1.5, -12 }, 2, mirror),
        sphere({ 1.5, -0.5, -18 }, 3, red_rubber),
        sphere({ 7, 5, -18 }, 4, mirror),
        //        sphere({ 0, 5, -16 }, 2, jade),
    };

    render(spheres, lights);
}
