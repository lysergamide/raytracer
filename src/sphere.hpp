#pragma once

#include <optional>

#include "./ray.hpp"
#include "./vec.hpp"
#include <fmt/core.h>

struct sphere {
  vec3f center;
  float radius;
  vec3f color;

  sphere(const vec3f c, const float r) : center(c), radius(r), color() {}
  sphere(const vec3f c, const float r, const vec3f col)
      : center(c)
      , radius(r)
      , color(col)
  {
  }

  /*
   * https://en.wikipedia.org/wiki/Line-sphere_intersection
   * returns the distance if we have a hit
   * otherwise nothing
   */

  auto
  ray_intersection(const ray &r) const -> std::optional<float>
  {
    auto oc = r.origin - this->center;
    auto a  = dot(r.dir, r.dir);
    auto b  = 2.0f * dot(oc, r.dir);
    auto c  = dot(oc, oc) - pow(this->radius, 2);

    auto disc = b * b - 4 * a * c;

    if (disc < 0) return {};

    return (-b - sqrtf(disc)) / (2.0f * a);
  }
};
