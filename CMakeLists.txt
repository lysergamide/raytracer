cmake_minimum_required(VERSION 3.14)
project(RayTracer)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(
  rayTracer
  ./src/main.cpp
  ./src/renderer.cpp
)