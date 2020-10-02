#pragma once

#include "vec.hpp"

// struct representing a light
struct light {
    vec3f origin;
    float intensity;

    light(const vec3f origin, const float intensity)
        : origin(origin)
        , intensity(intensity)
    {
    }
};
