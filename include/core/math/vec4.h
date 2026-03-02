#pragma once

namespace cas {

struct Vec4 {
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };

    constexpr Vec4() = default;
    constexpr Vec4(float x_, float y_, float z_, float w_)
        : x(x_), y(y_), z(z_), w(w_) {
    }
};

} // namespace cas