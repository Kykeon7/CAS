#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include <automata/automaton.h>
#include <graphics/color.h>
#include <math/vec2.h>
#include <platform/sdl_texture.h>
#include <platform/texture.h>
#include <rendering/automaton/config.h>
#include <rendering/automaton/renderer.h>
#include <viewports/camera.h>
#include <viewports/hex_layout.h>
#include <viewports/linear_history.h>

namespace cas::platform {
    class renderer;
}

namespace cas::rendering::automaton {

/**
 * Implements automaton rendering using CPU-based rasterization.
 *
 * Renders hexagonal automata by drawing their cells and grid directly
 * into a pixel buffer. Linear automata are rendered using their
 * generation history as a sequence of square cells.
 *
 * The renderer owns its output texture and pixel buffers, but does not
 * own the platform renderer passed to its constructor. The referenced
 * renderer must outlive this object.
 */
class hex_renderer final : public renderer {
public:
    /**
     * Creates a CPU automaton renderer.
     *
     * @param renderer Platform renderer used to manage the output texture.
     * @param layout Hexagonal layout used to calculate cell positions.
     */
    hex_renderer(
        cas::platform::renderer& renderer,
        cas::viewports::hex_layout layout
    );

    ~hex_renderer() override = default;

    hex_renderer(const hex_renderer&) = delete;
    hex_renderer& operator=(const hex_renderer&) = delete;
    hex_renderer(hex_renderer&&) = delete;
    hex_renderer& operator=(hex_renderer&&) = delete;

    /**
     * Resizes the viewport and its associated pixel buffer.
     *
     * Non-positive dimensions disable rendering until a valid size
     * is supplied.
     *
     * @param size New viewport dimensions in pixels.
     */
    void resize(const math::vec2i& size) override;

    /**
     * Renders an automaton using CPU-based rasterization.
     *
     * Hexagonal automata are rendered using their current cell states
     * and grid layout. Linear automata are rendered using their
     * generation history.
     *
     * @param automaton Automaton whose state is rendered.
     * @param history History of the automaton's generations.
     * @param config Visual configuration used for rendering.
     * @param camera Camera defining the visible world-space region.
     * @return Non-owning reference to the output texture, or std::nullopt
     *         if the viewport or texture is unavailable, or if a linear
     *         automaton has no history to render.
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<platform::texture>> render(
        const automata::automaton& automaton,
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    ) override;

private:
    /** Fills the pixel buffer with the specified pixel value. */
    void clear(std::uint32_t pixel);

    /** Rasterizes a filled hexagon into the pixel buffer. */
    void draw_hex(
        const math::vec2f& center,
        float size,
        std::uint32_t pixel
    );

    /** Rasterizes a hexagon outline into the pixel buffer. */
    void draw_hex_outline(
        const math::vec2f& center,
        float size,
        std::uint32_t pixel
    );

    /** Rasterizes a line between two points into the pixel buffer. */
    void draw_line(
        const math::vec2f& start,
        const math::vec2f& end,
        std::uint32_t pixel
    );

    /** Rasterizes a filled square into the pixel buffer. */
    void draw_square(
        const math::vec2f& center,
        float size,
        std::uint32_t pixel
    );

    /** Draws the living and persistent cells of a hexagonal automaton. */
    void draw_cells(
        const automata::automaton& automaton,
        const config& config,
        const cas::viewports::camera& camera
    );

    /** Draws the hexagonal grid of an automaton. */
    void draw_grid(
        const automata::automaton& automaton,
        const config& config,
        const cas::viewports::camera& camera
    );

    /** Draws the generation history of a linear automaton. */
    void draw_linear_history(
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    );

    /**
     * Converts a color into the packed 32-bit pixel format used by the
     * renderer's pixel buffer.
     *
     * @param color Color to convert.
     * @return Packed 32-bit pixel value.
     */
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