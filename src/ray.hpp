#pragma once

#include "./vec.hpp"

struct ray {
  vec3f origin;
  vec3f dir;

  ray() {}
  ray(vec3f o, vec3f d) : origin(o), dir(d) {}
};
