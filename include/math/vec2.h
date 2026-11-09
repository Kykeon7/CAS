#pragma once

#include <cmath>

namespace cas::math {

template <typename T>
struct vec2 {
    T x{};
    T y{};

    constexpr vec2() = default;

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

    float length() const {
        return std::sqrt(
            static_cast<float>(x * x + y * y)
        );
    }

    static constexpr vec2 zero() {
        return { 0, 0 };
    }
};

using vec2f = vec2<float>;
using vec2i = vec2<int>;

} // namespace cas