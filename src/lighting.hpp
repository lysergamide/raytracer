#pragma once

#include "./vec.hpp"

// struct representing a light
struct light {
  vec3f position;
  float intensity;

  light(const vec3f position, const float intensity)
      : position(position)
      , intensity(intensity)
  {
  }
};
