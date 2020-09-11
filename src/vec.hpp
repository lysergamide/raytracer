#pragma once

#include <array>
#include <cmath>
#include <type_traits>

#include <fmt/format.h>

template <typename T>
concept Arithmetic = std::is_arithmetic<T>::value;

/*
 * Vector and associated functions
 */
template <size_t N, typename T>
using vec = std::array<T, N>;

using vec3f = vec<3, float>;

// addition
template <size_t N, typename T>
auto operator+(vec<N, T> a, const vec<N, T> b) -> vec<N, T>
{
  for (size_t i = 0; i < N; i++) a[i] += b[i];
  return a;
}

// subtraction
template <size_t N, typename T>
auto operator-(vec<N, T> a, const vec<N, T> b) -> vec<N, T>
{
  for (size_t i = 0; i < N; i++) a[i] -= b[i];
  return a;
}

// cross product
template <size_t N, typename T>
auto cross(const vec<N, T> a, const vec<N, T> b) -> vec<N, T>
{
  auto ret = vec<N, T> {};

  for (size_t i = 1; i <= N; i++) {
    auto lhs = a[i % N] * b[i + 1 % N];
    auto rhs = a[i + 1 % N] * b[i % N];

    ret[i - 1] = lhs - rhs;
  }

  return ret;
}

// dot product
template <size_t N, typename T>
auto dot(const vec<N, T> a, const vec<N, T> b) -> T
{
  auto ret = T {};

  for (size_t i = 0; i < N; i++) ret += a[i] * b[i];

  return ret;
}

// scalar product
template <size_t N, typename T, Arithmetic S>
auto operator*(vec<N, T> v, const S c) -> vec<N, T>
{
  for (size_t i = 0; i < N; i++) v[i] *= c;
  return v;
}

template <size_t N, typename T, Arithmetic S>
auto operator*(const S c, vec<N, T> v) -> vec<N, T>
{
  return v * c;
}

template <size_t N, typename T>
auto operator-(vec<N, T> v) -> vec<N, T>
{
  return v * -1;
}

// Magnitude
template <size_t N, typename T>
auto mag(vec<N, T> v) -> float
{
  auto ret = 0.0F;

  for (size_t i = 0; i < N; i++) ret += v[i] * v[i];

  return std::sqrt(ret);
}

// Normalize vector, get its directional vector
template <size_t N, typename T>
auto normalize(vec<N, T> v) -> vec<N, T>
{
  return v * (1.0 / mag(v));
}

// Projection of v onto u
template <size_t N, typename T>
auto proj(vec<N, T> v, vec<N, T> u) -> vec<N, T>
{
  auto mu = mag(u);
  return (dot(u, v) / (mu * mu)) * u;
}

// TODO
template <size_t N, typename T>
auto reflect(const vec<N, T>& a, const vec<N, T>& b) -> vec<N, T>
{
  return a - b * 2.f * dot(a, b);
}

/*
 * Taken from fmt docs, makes the vectors printable for debugging
 */
template <>
struct fmt::formatter<vec3f> {
  // Presentation format: 'f' - fixed, 'e' - exponential.
  char presentation = 'f';

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto parse(format_parse_context& ctx)
  {
    // auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) // c++11
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - point of interest", point{1, 2});
    //
    // the range will contain "f} - point of interest". The formatter should
    // parse specifiers until '}' or the end of the range. In this example
    // the formatter should parse the 'f' specifier and return an iterator
    // pointing to '}'.

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e')) presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}') throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the point p using the parsed format specification (presentation)
  // stored in this formatter.
  template <typename FormatContext>
  auto format(const vec3f& v, FormatContext& ctx)
  {
    // auto format(const point &p, FormatContext &ctx) -> decltype(ctx.out()) //
    // c++11 ctx.out() is an output iterator to write to.
    return format_to(ctx.out(),
                     presentation == 'f' ? "<{:.1f}, {:.1f}, {:.1f}>"
                                         : "<{:.1e}, {:.1e}, {:.1e}>",
                     v[0],
                     v[1],
                     v[2]);
  }
};
