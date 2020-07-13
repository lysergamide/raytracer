#include <array>
#include <fstream>
#include <vector>

#include <fmt/format.h>  // Compile this with -lfmt

constexpr unsigned width  = 1024;
constexpr unsigned height = 768;

using namespace std;
using namespace fmt;

using vec3f = std::array<float, 3>;  // Extremly simple math vector

/*
 * Outputs a red/green gradient in a simple image format (ppm)
 */

auto
main() -> int
{
  auto fbuffer = vector<vec3f>(width * height);

  for (size_t j = 0; j < height; j++) {
    for (size_t i = 0; i < width; i++) {
      fbuffer[i + j * width] = vec3f { static_cast<float>(j) / height,
                                       static_cast<float>(i) / width,
                                       0.0f };
    }
  }

  auto ofs = ofstream("./out.ppm");
  ofs << fmt::format("P6\n{} {}\n255\n", width, height);

  for (const auto vec : fbuffer)
    for (const auto val : vec)
      ofs << static_cast<char>(255 * max(0.0f, min(1.0f, val)));
}
