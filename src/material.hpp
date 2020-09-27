#include "./vec.hpp"

/**
 * Material struct that describes objects color and how matte or shiny it is
 */
struct material {
  vec3f color;
  float idiff;
  float ispec;
  float spec_exp;

  /**
   * @brief Construct a new material object
   *
   * @param color material color
   * @param idiff diffuse intensity
   * @param ispec specular intensity
   * @param spec_exp specular exponent
   */
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
