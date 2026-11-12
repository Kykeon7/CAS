#pragma once

#include <cmath>

namespace cas::math {

/**
 * Represents a two-dimensional vector with generic component types.
 *
 * @tparam T Component type.
 */
template <typename T>
struct vec2 {
    T x{};
    T y{};

    constexpr vec2() = default;

    constexpr vec2(const vec2&) = default;
    constexpr vec2& operator=(const vec2&) = default;

    constexpr vec2(vec2&&) = default;
    constexpr vec2& operator=(vec2&&) = default;

    ~vec2() = default;

    constexpr vec2(T x_, T y_)
        : x(x_), y(y_)
    {
    }

    template <typename U>
    explicit constexpr operator vec2<U>() const {
        return {
            static_cast<U>(x),
            static_cast<U>(y)
        };
    }

    constexpr vec2 operator+(const vec2& v) const {
        return { x + v.x, y + v.y };
    }

    constexpr vec2 operator-(const vec2& v) const {
        return { x - v.x, y - v.y };
    }

    constexpr vec2 operator*(T scalar) const {
        return { x * scalar, y * scalar };
    }

    constexpr vec2 operator/(T scalar) const {
        return { x / scalar, y / scalar };
    }

    constexpr vec2& operator+=(const vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr vec2& operator-=(const vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr vec2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /** Returns the Euclidean length of the vector. */
    float length() const {
        return std::hypot(
            static_cast<float>(x),
            static_cast<float>(y)
        );
    }

    /** Returns a vector with both components initialized to zero. */
    static constexpr vec2 zero() {
        return { 0, 0 };
    }
};

using vec2f = vec2<float>;
using vec2i = vec2<int>;

} // namespace cas::math