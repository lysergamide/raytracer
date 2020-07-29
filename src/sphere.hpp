#pragma once
#include "./geometry.hpp"

/*
 * Sphere
 */

struct sphere {
 public:
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

  // https://en.wikipedia.org/wiki/Line-sphere_intersection
  auto
  ray_intersection(const vec3f &ray) const -> bool
  {
    auto ray_dir = normalize(ray);
    auto rs      = this->center - ray;

    auto a = dot(ray_dir, ray_dir);
    auto b = 2 * dot(rs, ray_dir);
    auto c = dot(rs, rs) - this->radius * this->radius;

    return b * b - 4 * a * c >= 0;
  }
};
