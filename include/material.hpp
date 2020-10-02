#include "vec.hpp"

/**
 * Material struct that describes objects color and how matte or shiny it is
 */
struct material {
    // [red, green, blue]
    vec3f color;
    // [diffuse intensity, specular intensity, reflectiveness, refractivness]
    vec<4, float> albedo;

    float spec_exp;
    float refractive_i;

    /**
     * @brief Construct a new material object
     *
     * @param color material color
     * @param albedo [diffuse intensity, specular intensity, reflectiveness,
     * refractiveness]
     * @param spec_exp specular exponent
     */
    material(const vec3f&         color,
             const vec<4, float>& albedo,
             const float          spec_exp,
             const float          refractive_i)
        : color(color)
        , albedo(albedo)
        , spec_exp(spec_exp)
        , refractive_i(refractive_i)
    {
    }
};
