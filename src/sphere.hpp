#pragma once

#include <optional>

#include "./material.hpp"
#include "./ray.hpp"
#include "./vec.hpp"

struct sphere {
    vec3f    center;
    float    radius;
    material mat;

    /**
     * @brief Construct a new sphere object
     *
     * @param c sphere center or orgin
     * @param r sphere radius
     * @param m sphere material
     */
    sphere(const vec3f c, const float r, const material m)
        : center(c)
        , radius(r)
        , mat(m)
    {
    }

    /**
     * @brief https://en.wikipedia.org/wiki/Line-sphere_intersection
     *
     * @param r ray being checked for interestion
     * @return std::optional<float> distance between the ray origin and sphere
     * if we hit
     */
    auto ray_intersection(const ray& r) const -> std::optional<float>
    {
        auto oc = r.origin - this->center;
        auto a  = dot(r.dir, r.dir);
        auto b  = 2.0f * dot(oc, r.dir);
        auto c  = dot(oc, oc) - pow(this->radius, 2);

        auto disc = b * b - 4 * a * c;

        if (disc < 0)
            return {};

        return (-b - sqrtf(disc)) / (2.0f * a);
    }
};
