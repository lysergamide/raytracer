#include "./vec.hpp"

/**
 * Material struct that describes objects color and how matte or shiny it is
 */
struct material {
    vec3f color;
    vec3f albedo;  //[diffuse intensity, specular intensity, reflectiveness]
    float spec_exp;

    /**
     * @brief Construct a new material object
     *
     * @param color material color
     * @param albedo [diffuse intensity, specular intensity, reflectiveness]
     * @param spec_exp specular exponent
     */
    material(const vec3f& color, const vec3f& albedo, const float spec_exp)
        : color(color)
        , albedo(albedo)
        , spec_exp(spec_exp)
    {
    }
};
