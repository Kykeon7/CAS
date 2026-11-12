#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>

#include <automata/automaton.h>
#include <math/vec2.h>
#include <platform/texture.h>
#include <rendering/automaton/config.h>
#include <rendering/automaton/renderer.h>
#include <viewports/camera.h>
#include <viewports/linear_history.h>

namespace cas::platform {
    class gpu_renderer;
}

namespace cas::rendering::automaton {

/**
 * Implements automaton rendering using the GPU backend.
 *
 * Supports rendering regular automaton states and linear automaton
 * histories through the associated platform GPU renderer.
 *
 * The renderer does not own the GPU renderer passed to its constructor.
 * The referenced renderer must outlive this object.
 */
class gpu_renderer final : public renderer {
public:
    /**
     * Creates a GPU automaton renderer.
     *
     * @param renderer Platform GPU renderer used for rendering.
     */
    explicit gpu_renderer(
        platform::gpu_renderer& renderer
    ) noexcept;

    ~gpu_renderer() override = default;

    gpu_renderer(const gpu_renderer&) = delete;
    gpu_renderer& operator=(const gpu_renderer&) = delete;
    gpu_renderer(gpu_renderer&&) = delete;
    gpu_renderer& operator=(gpu_renderer&&) = delete;

    /**
     * Resizes the viewport used by the renderer.
     *
     * If either dimension is non-positive, the viewport size is reset
     * and subsequent rendering attempts return no texture.
     *
     * @param size New viewport dimensions in pixels.
     */
    void resize(const math::vec2i& size) override;

    /**
     * Renders an automaton using the GPU backend.
     *
     * Linear automata are rendered using their generation history,
     * while other topologies are rendered using their current state.
     *
     * @param automaton Automaton whose state is rendered.
     * @param history History of the automaton's generations.
     * @param config Visual configuration used for rendering.
     * @param camera Camera defining the visible world-space region.
     * @return Non-owning reference to the output texture, or std::nullopt
     *         if the viewport, output texture, or required history is
     *         unavailable.
     * @throws std::runtime_error If the automaton dimensions are invalid
     *         or its storage size does not match those dimensions.
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<platform::texture>> render(
        const automata::automaton& automaton,
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    ) override;

private:
    platform::gpu_renderer& _renderer;
    math::vec2i _viewport_size{};

    std::vector<std::uint8_t> _linear_history_upload;
};

} // namespace cas::rendering::automaton