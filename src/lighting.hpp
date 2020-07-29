#pragma once

#include "./vec.hpp"

struct light {
  vec3f position;
  float intensity;

  light(const vec3f p, const float i) : position(p), intensity(i) {}
};
