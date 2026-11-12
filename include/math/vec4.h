#pragma once

namespace cas::math {

/** Represents a four-dimensional vector of floating-point components. */
struct vec4 {
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };

    constexpr vec4() = default;

    constexpr vec4(const vec4&) = default;
    constexpr vec4& operator=(const vec4&) = default;

    constexpr vec4(vec4&&) = default;
    constexpr vec4& operator=(vec4&&) = default;

    ~vec4() = default;

    constexpr vec4(float x_, float y_, float z_, float w_)
        : x(x_), y(y_), z(z_), w(w_)
    {
    }
};

} // namespace cas::math