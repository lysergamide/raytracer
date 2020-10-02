#pragma once

#include <array>
#include <cmath>

/*
 * math vector and associated functions
 */

template <size_t N, typename T>
using vec = std::array<T, N>;

// a vec of 3 floats
using vec3f = vec<3, float>;

// addition
template <size_t N, typename T>
auto operator+(vec<N, T> a, const vec<N, T>& b) -> vec<N, T>
{
    for (size_t i = 0; i < N; i++)
        a[i] += b[i];

    return a;
}

// subtraction
template <size_t N, typename T>
auto operator-(vec<N, T> a, const vec<N, T>& b) -> vec<N, T>
{
    for (size_t i = 0; i < N; i++)
        a[i] -= b[i];

    return a;
}

// cross product
template <size_t N, typename T>
auto cross(const vec<N, T>& a, const vec<N, T>& b) -> vec<N, T>
{
    auto ret = vec<N, T>{};

    for (size_t i = 1; i <= N; i++) {
        auto lhs = a[i % N] * b[i + 1 % N];
        auto rhs = a[i + 1 % N] * b[i % N];

        ret[i - 1] = lhs - rhs;
    }

    return ret;
}

// dot product
template <size_t N, typename T>
auto dot(const vec<N, T>& a, const vec<N, T>& b) -> T
{
    auto ret = T{};

    for (size_t i = 0; i < N; i++)
        ret += a[i] * b[i];

    return ret;
}

// scalar product
template <size_t N, typename T, typename S>
auto operator*(vec<N, T> v, const S c) -> vec<N, T>
{
    for (size_t i = 0; i < N; i++)
        v[i] *= c;

    return v;
}

// scalar product
template <size_t N, typename T, typename S>
auto operator*(const S c, const vec<N, T>& v) -> vec<N, T>
{
    return v * c;
}

// return -v
template <size_t N, typename T>
auto operator-(const vec<N, T>& v) -> vec<N, T>
{
    return v * -1;
}

// Magnitude, also called 'norm'
template <size_t N, typename T>
auto magnitude(const vec<N, T>& v) -> float
{
    auto ret = 0.0F;

    for (size_t i = 0; i < N; i++)
        ret += v[i] * v[i];

    return std::sqrt(ret);
}

// Normalize vector, get its directional vector
template <size_t N, typename T>
auto normalize(const vec<N, T>& v) -> vec<N, T>
{
    return v * (1.0 / magnitude(v));
}

// Projection of v onto u
template <size_t N, typename T>
auto proj(const vec<N, T>& v, const vec<N, T>& u) -> vec<N, T>
{
    auto mu = magnitude(u);
    return (dot(u, v) / (mu * mu)) * u;
}