#pragma once

#include <cmath>

namespace cas {

struct Vec2 {
    float x{ 0.f };
    float y{ 0.f };

    constexpr Vec2() = default;
    constexpr Vec2(float x_, float y_) : x(x_), y(y_) {}

    constexpr Vec2 operator+(const Vec2& v) const {
        return { x + v.x, y + v.y };
    }

    constexpr Vec2 operator-(const Vec2& v) const {
        return { x - v.x, y - v.y };
    }

    constexpr Vec2 operator*(float s) const {
        return { x * s, y * s };
    }

    constexpr Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    static constexpr Vec2 zero() { return { 0.f, 0.f }; }
};

}