#pragma once

#include <cstdint>
#include <algorithm>

namespace cas {

struct Color {

    float r{ 0.f };
    float g{ 0.f };
    float b{ 0.f };
    float a{ 1.f };

    constexpr Color() = default;

    constexpr Color(float r_, float g_, float b_, float a_ = 1.f)
        : r(std::clamp(r_, 0.f, 1.f)),
        g(std::clamp(g_, 0.f, 1.f)),
        b(std::clamp(b_, 0.f, 1.f)),
        a(std::clamp(a_, 0.f, 1.f)) {
    }

    static Color from_rgba8(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        return {
            r / 255.f,
            g / 255.f,
            b / 255.f,
            a / 255.f
        };
    }

    static Color from_hex(uint32_t hex) {
        uint8_t r = (hex >> 24) & 0xFF;
        uint8_t g = (hex >> 16) & 0xFF;
        uint8_t b = (hex >> 8) & 0xFF;
        uint8_t a = (hex) & 0xFF;
        return from_rgba8(r, g, b, a);
    }
};

}