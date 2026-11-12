#pragma once

#include <algorithm>
#include <cstdint>

namespace cas::graphics {

/// @brief Represents an RGBA color with normalized floating-point channels.
struct color {
    float r{ 0.f };
    float g{ 0.f };
    float b{ 0.f };
    float a{ 1.f };

    constexpr color() = default;

    constexpr color(const color&) = default;
    constexpr color& operator=(const color&) = default;

    constexpr color(color&&) noexcept = default;
    constexpr color& operator=(color&&) noexcept = default;

    ~color() = default;

    /// @brief Constructs a color from normalized RGBA channels.
    ///
    /// Each channel is clamped to the range [0, 1].
    constexpr color(
        float r_,
        float g_,
        float b_,
        float a_ = 1.f
    ) :
        r(std::clamp(r_, 0.f, 1.f)),
        g(std::clamp(g_, 0.f, 1.f)),
        b(std::clamp(b_, 0.f, 1.f)),
        a(std::clamp(a_, 0.f, 1.f))
    {
    }

    /// @brief Constructs a color from 8-bit RGBA channels.
    [[nodiscard]]
    static constexpr color from_rgba8(
        std::uint8_t r,
        std::uint8_t g,
        std::uint8_t b,
        std::uint8_t a = 255
    ) {
        return {
            r / 255.f,
            g / 255.f,
            b / 255.f,
            a / 255.f
        };
    }

    /// @brief Constructs a color from a 32-bit RGBA hexadecimal value.
    ///
    /// The expected format is 0xRRGGBBAA.
    [[nodiscard]]
    static constexpr color from_hex(std::uint32_t hex) {
        const auto r = static_cast<std::uint8_t>((hex >> 24) & 0xFFu);
        const auto g = static_cast<std::uint8_t>((hex >> 16) & 0xFFu);
        const auto b = static_cast<std::uint8_t>((hex >> 8) & 0xFFu);
        const auto a = static_cast<std::uint8_t>(hex & 0xFFu);

        return from_rgba8(r, g, b, a);
    }
};

} // namespace cas::graphics