#include <vector>

#include "renderer.hpp"

using namespace std;

auto main() -> int
{
    const auto ivory = material({ 0.4, 0.4, 0.3 }, { 0.6, 0.3, 0.1, 0 }, 50, 1);
    const auto red_rubber = material({ 0.3, 0.1, 0.1 }, { 0.9, 0.1, 0 }, 10, 1);
    const auto mirror = material({ 1, 1, 1 }, { 0.01, 10, 0.85, 0 }, 1425, 1);
    const auto jade = material({ 0.2, 0.6, 0.5 }, { 0.5, 0.8, .2, 0 }, 1000, 1);
    const auto glass =
      material({ 0.6, 0.7, 0.8 }, { 0.0, 0.5, 0.1, 0.8 }, 125, 2.1);

    const auto spheres = vector<sphere>{
        sphere({ -3, 0, -16 }, 2, ivory),
        sphere({ -1, -1.5, -12 }, 2, glass),
        sphere({ 1.5, -0.5, -18 }, 3, red_rubber),
        sphere({ 7, 5, -18 }, 4, mirror),
        sphere({ 0, 5, -16 }, 2, jade),
    };

    const auto lights = vector<light>{
        light({ -20, 20, 20 }, 1.5),
        light({ 30, 50, -25 }, 1.8),
        light({ 30, 20, 30 }, 1.7),
    };

    render(spheres, lights);
}
