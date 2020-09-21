#include "./vec.hpp"

struct material {
    vec3f         color;
    vec<2, float> brightness;
    float         specular_exp;

    material() : color(), brightness({ 1, 0 }), specular_exp() {}

    material(const vec3f c, const vec<2, float> b, const float se)
        : color(c)
        , brightness(b)
        , specular_exp(se)
    {
    }
};
