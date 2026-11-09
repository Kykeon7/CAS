#pragma once

#include <automata/automaton.h>
#include <viewports/camera.h>
#include <graphics/color.h>
#include <math/vec2.h>
#include <rendering/automaton/renderer.h>
#include <viewports/hex_layout.h>
#include <platform/sdl_texture.h>
#include <platform/texture.h>
#include <cstdint>
#include <optional>
#include <vector>

namespace cas::rendering::automaton {

class hex_renderer final : public renderer {
public:
    hex_renderer(
        cas::platform::renderer& renderer,
        cas::viewports::hex_layout layout
    );

    void resize(const math::vec2i& size) override;

    [[nodiscard]]
    std::optional<std::reference_wrapper<platform::texture>> render(
        const automata::automaton& automaton,
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    ) override;

private:
    void clear(std::uint32_t pixel);

    void draw_hex(
        const math::vec2f& center,
        float size,
        std::uint32_t pixel
    );

    void draw_hex_outline(
        const math::vec2f& center,
        float size,
        std::uint32_t pixel
    );

    void draw_line(
        const math::vec2f& start,
        const math::vec2f& end,
        std::uint32_t pixel
    );

    void draw_square(
        const math::vec2f& center,
        float size,
        std::uint32_t pixel
    );

    void draw_cells(
        const automata::automaton& automaton,
        const config& config,
        const cas::viewports::camera& camera
    );

    void draw_grid(
        const automata::automaton& automaton,
        const config& config,
        const cas::viewports::camera& camera
    );

    void draw_linear_history(
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    );

    [[nodiscard]]
    static std::uint32_t to_rgba32(const graphics::color& color);

    cas::platform::renderer& _renderer;
    cas::platform::sdl_texture _texture;
    cas::viewports::hex_layout _layout;
    math::vec2i _size{};
    std::vector<std::uint32_t> _pixels;

    std::vector<std::uint8_t> _history_cells;
};

} // namespace cas::rendering::automaton