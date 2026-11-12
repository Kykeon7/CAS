#pragma once

#include <automata/automaton.h>
#include <platform/texture.h>
#include <rendering/automaton/config.h>
#include <viewports/camera.h>
#include <viewports/linear_history.h>

#include <functional>
#include <optional>

namespace cas::rendering::automaton {

/**
 * Defines the interface for rendering cellular automata.
 *
 * Provides operations for resizing the rendering resources and
 * rendering an automaton using its history, visual configuration,
 * and camera.
 */
class renderer {
public:
    virtual ~renderer() = default;

    renderer() = default;

    renderer(const renderer&) = delete;
    renderer& operator=(const renderer&) = delete;
    renderer(renderer&&) = delete;
    renderer& operator=(renderer&&) = delete;

    /**
     * Resizes the resources used by the renderer.
     *
     * @param size New dimensions in pixels.
     */
    virtual void resize(const math::vec2i& size) = 0;

    /**
     * Renders an automaton using the supplied state and view parameters.
     *
     * @param automaton Automaton to render.
     * @param history History of the automaton's states.
     * @param config Visual configuration used for rendering.
     * @param camera Camera defining the visible world-space region.
     * @return Reference to the rendered texture, or std::nullopt if
     *         no texture is available.
     */
    [[nodiscard]]
    virtual std::optional<std::reference_wrapper<platform::texture>> render(
        const automata::automaton& automaton,
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    ) = 0;
};

} // namespace cas::rendering::automaton