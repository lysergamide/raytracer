#pragma once
#include <vector>

#include "geometry.hpp"

auto render(const std::vector<sphere>& spheres,
            const std::vector<light>&  lights) -> void;
