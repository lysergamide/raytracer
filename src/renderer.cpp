#include <algorithm>
#include <cmath>
#include <fstream>
#include <functional>
#include <numeric>
#include <vector>

#include "geometry.hpp"

#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;
constexpr float    fov    = M_PI / 2.f;  // 90 degrees

auto reflect(const vec3f& l, const vec3f& n) -> vec3f
{
    return 2.f * dot(l, n) * n - l;
}

// snell's law
auto refract(const vec3f& l,
             const vec3f& n,
             const float  n1,
             const float  n2 = 1.f) -> vec3f
{
    auto c = dot(-n, l);
    if (c < 0) return refract(l, -n, n2, n1);
    auto r = n2 / n1;
    auto k = 1 - r * r * (1 - c * c);

    if (k < 0)
        return { 1, 0, 0 };
    else
        return normalize(r * l + ((r * c - sqrt(k)) * n));
}

auto cast_ray(const ray&                 r,
              const std::vector<sphere>& spheres,
              const std::vector<light>&  lights,
              const size_t               depth = 0) -> vec3f
{
    using namespace std;

    constexpr auto default_color = vec3f{ 0.2, 0.7, 0.8 };
    // depth limit guard
    if (depth > 4) return default_color;

    auto dist = numeric_limits<float>::max();
    auto sphr = optional<sphere>{};

    // find nearest sphere if it exists
    for (const auto& s : spheres) {
        const auto d = s.ray_intersection(r);

        if (d && d.value() > 0 && d.value() < dist) {
            dist = d.value();
            sphr = s;
        }
    }

    if (!sphr) return default_color;

    /********************
     * Phong reflection *
     ********************/

    // point on sphere
    const auto point = r.origin + (r.dir * dist);
    // direction from center of sphere to point
    const auto  normv = normalize(point - sphr->center);
    const auto& mat   = sphr->mat;

    auto diff = 0.f;
    auto spec = 0.f;

    for (const auto& l : lights) {
        const auto l_dir  = normalize(l.origin - point);
        const auto l_dist = magnitude(l.origin - point) - 0.1;
        const auto l_ray  = ray{ l.origin, -l_dir };

        // helper function, returns true if light is blocked
        const auto blocks = [&](const sphere& s) -> bool {
            return l_dist > s.ray_intersection(l_ray).value_or(
                     numeric_limits<float>::max());
        };
        // skip current light if it's blocked by another object
        if (any_of(begin(spheres), end(spheres), blocks)) continue;

        // direction of reflected light
        const auto ref_dir = reflect(-l_dir, normv);

        diff += l.intensity * max(0.f, dot(l_dir, normv));
        spec += powf(max(0.f, dot(ref_dir, r.dir)), mat.spec_exp) * l.intensity;
    }

    const auto diff_color = mat.color * diff * mat.albedo[0];
    const auto spec_color = vec3f{ 1, 1, 1 } * spec * mat.albedo[1];

    // helper function for reflection and refraction
    const auto recast = [&](const auto& dir) {
        const auto origin = dot(dir, normv) < 0 ? point - normv * 0.0001
                                                : point + normv * 0.0001;
        const auto new_ray = ray(origin, dir);

        return cast_ray(new_ray, spheres, lights, depth + 1);
    };

    const auto reflect_color = recast(reflect(-r.dir, normv)) * mat.albedo[2];
    const auto refract_color = recast(refract(r.dir, normv, mat.refractive_i))
                               * mat.albedo[3];

    return diff_color + spec_color + reflect_color + refract_color;
}

auto render(const std::vector<sphere>& spheres,
            const std::vector<light>&  lights) -> void
{
    using namespace std;
    using namespace std::placeholders;

    constexpr auto fwidth  = static_cast<float>(width);
    constexpr auto fheight = static_cast<float>(height);
    const auto     angle   = tanf(fov / 2.f);
    const auto     origin  = vec3f{ 0, 0, 0 };

    auto buffer = vector<vec3f>(width * height);

    const auto build_ray = [&](auto h, auto w) {
        const auto x = (2 * (w + .5f) / fwidth - 1) * angle * fwidth / fheight;
        const auto y = -(2 * (h + .5f) / fheight - 1) * angle;
        return ray{ origin, normalize(vec3f{ x, y, -1 }) };
    };

    const auto cast = bind(cast_ray, _1, spheres, lights, 0);

#pragma omp parallel for
    for (size_t h = 0; h < height; h++)
        for (size_t w = 0; w < width; w++)
            buffer[w + (h * width)] = cast(build_ray(h, w));

    // will overwrite existing file
    auto ofs = ofstream("./out.ppm", ios::trunc | ios::binary);

    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (const auto& vec : buffer)
        for (const auto& val : vec)
            ofs << static_cast<char>(255 * max(0.f, min(1.f, val)));
    ofs.close();
}