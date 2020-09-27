#pragma once

#include "./vec.hpp"

// struct representing a ray
struct ray {
  vec3f origin;
  vec3f dir;
  ray(vec3f origin, vec3f direction) : origin(origin), dir(direction) {}
};
