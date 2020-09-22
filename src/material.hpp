#include "./vec.hpp"

struct material {
  vec3f color;     // material colo
  float idiff;     // diffuse intensity
  float ispec;     // specular intensity
  float spec_exp;  // specular exponent

  material() : color(), idiff(), ispec(), spec_exp() {}

  material(const vec3f& color,
           const float  idiff,
           const float  ispec,
           const float  spec_exp)
      : color(color)
      , idiff(idiff)
      , ispec(ispec)
      , spec_exp(spec_exp)
  {
  }
};
