
#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <inputs/input.h>
#include <math/vec2.h>
#include <rendering/automaton/config.h>
#include <rendering/automaton/renderer.h>
#include <simulations/simulation.h>
#include <viewports/camera.h>
#include <viewports/hex_layout.h>
#include <viewports/history_buffer.h>
#include <viewports/linear_history.h>

namespace cas::viewports {

/**
 * Represents a simulation viewport and its associated rendering state.
 *
 * Manages the simulation, camera, visual configuration, renderer,
 * and recorded simulation histories. Handles user input for cell
 * interaction, camera movement, and zoom.
 *
 * The viewport exclusively owns its automaton renderer.
 */
class viewport {
public:
    /**
     * Creates a viewport with an optional automaton renderer.
     *
     * @param renderer Renderer to take ownership of, or nullptr if
     *                 rendering is not yet available.
     */
    explicit viewport(
        std::unique_ptr<cas::rendering::automaton::renderer> renderer = nullptr
    );

    /**
     * Processes user input directed at this viewport.
     *
     * Input is ignored when the viewport is not hovered. Left mouse
     * clicks cycle the state of the corresponding hexagonal cell,
     * right mouse dragging moves the camera, and mouse wheel input
     * zooms around the cursor position.
     *
     * @param input Input state to process.
     * @param is_hovered Whether the viewport is currently hovered.
     * @param viewport_pos Viewport position in screen coordinates.
     */
    void handle_input(
        const inputs::input& input,
        bool is_hovered,
        math::vec2f viewport_pos
    );

    /**
     * Advances the simulation when its configured update interval
     * has elapsed.
     *
     * Records linear automaton history and updates population,
     * density, and activity histories after each simulation step.
     */
    void update_sim();

    /**
     * Renders the current simulation state.
     *
     * Records linear automaton history before invoking the renderer.
     *
     * @return Non-owning reference to the rendered texture, or
     *         std::nullopt if the renderer cannot produce one.
     * @throws std::invalid_argument if no renderer is assigned.
     */
    [[nodiscard]]
    std::optional<std::reference_wrapper<platform::texture>>
    render();

    /**
     * Converts viewport-local screen coordinates into world coordinates.
     *
     * @param local Position relative to the viewport's top-left corner.
     * @return Corresponding world-space position.
     */
    [[nodiscard]]
    math::vec2f screen_to_world(const math::vec2f& local) const;

    /**
     * Returns mutable access to the simulation.
     *
     * @return Reference to the viewport simulation.
     */
    cas::simulations::simulation& simulation() {
        return _simulation;
    }

    /**
     * Returns read-only access to the simulation.
     *
     * @return Constant reference to the viewport simulation.
     */
    const cas::simulations::simulation& simulation() const {
        return _simulation;
    }

    /**
     * Returns mutable access to the hexagonal layout.
     *
     * @return Reference to the viewport layout.
     */
    hex_layout& layout() {
        return _layout;
    }

    /**
     * Returns mutable access to the rendering configuration.
     *
     * @return Reference to the active visual configuration.
     */
    cas::rendering::automaton::config& config() {
        return _config;
    }

    /**
     * Returns read-only access to the rendering configuration.
     *
     * @return Constant reference to the active visual configuration.
     */
    const cas::rendering::automaton::config& config() const {
        return _config;
    }

    /**
     * Returns mutable access to the preview rendering configuration.
     *
     * @return Reference to the preview visual configuration.
     */
    cas::rendering::automaton::config& preview_config() {
        return _preview_config;
    }

    /**
     * Returns read-only access to the preview rendering configuration.
     *
     * @return Constant reference to the preview visual configuration.
     */
    const cas::rendering::automaton::config& preview_config() const {
        return _preview_config;
    }

    /**
     * Returns mutable access to the population history.
     *
     * @return Reference to the population history buffer.
     */
    history_buffer& population_history() {
        return _population_history;
    }

    /**
     * Returns read-only access to the population history.
     *
     * @return Constant reference to the population history buffer.
     */
    const history_buffer& population_history() const {
        return _population_history;
    }

    /**
     * Returns mutable access to the density history.
     *
     * @return Reference to the density history buffer.
     */
    history_buffer& density_history() {
        return _density_history;
    }

    /**
     * Returns read-only access to the density history.
     *
     * @return Constant reference to the density history buffer.
     */
    const history_buffer& density_history() const {
        return _density_history;
    }

    /**
     * Returns mutable access to the activity history.
     *
     * @return Reference to the activity history buffer.
     */
    history_buffer& activity_history() {
        return _activity_history;
    }

    /**
     * Returns read-only access to the activity history.
     *
     * @return Constant reference to the activity history buffer.
     */
    const history_buffer& activity_history() const {
        return _activity_history;
    }

    /**
     * Returns mutable access to the linear automaton history.
     *
     * @return Reference to the linear history.
     */
    [[nodiscard]]
    linear_history& linear_history_data() noexcept {
        return _linear_history;
    }

    /**
     * Returns read-only access to the linear automaton history.
     *
     * @return Constant reference to the linear history.
     */
    [[nodiscard]]
    const linear_history& linear_history_data() const noexcept {
        return _linear_history;
    }

    /**
     * Returns mutable access to the camera.
     *
     * @return Reference to the viewport camera.
     */
    cas::viewports::camera& camera() {
        return _camera;
    }

    /**
     * Returns read-only access to the camera.
     *
     * @return Constant reference to the viewport camera.
     */
    const cas::viewports::camera& camera() const {
        return _camera;
    }

    /**
     * Updates the viewport dimensions.
     *
     * Also updates the camera viewport size and resizes the assigned
     * renderer, if present.
     *
     * @param size New viewport dimensions in pixels.
     */
    void set_size(const math::vec2i& size);

    /**
     * Returns the viewport's display name.
     *
     * @return Constant reference to the viewport name.
     */
    [[nodiscard]]
    const std::string& name() const noexcept;

    /**
     * Sets the viewport's display name.
     *
     * Empty names are ignored.
     *
     * @param name New viewport name.
     */
    void set_name(std::string name);

    /**
     * Assigns a new automaton renderer.
     *
     * Takes ownership of the supplied renderer. If the pointer is
     * nullptr, the current renderer is left unchanged. Otherwise,
     * the new renderer is resized when the viewport dimensions
     * are positive.
     *
     * @param renderer Renderer to take ownership of.
     */
    void set_renderer(
        std::unique_ptr<cas::rendering::automaton::renderer> renderer
    );

    /**
     * Zooms in around the center of the viewport.
     */
    void zoom_in();

    /**
     * Zooms out around the center of the viewport.
     */
    void zoom_out();

    /**
     * Resets the camera zoom to its default value.
     */
    void zoom_reset();

    /**
     * Resets the camera position to the world origin.
     */
    void reset_position();

private:
    /**
     * Changes the camera zoom while keeping the specified screen
     * position anchored to the same world-space point.
     *
     * The resulting zoom factor is clamped to the range [0.1, 10.0].
     *
     * @param local Anchor position in viewport-local screen coordinates.
     * @param factor Multiplicative zoom factor.
     */
    void zoom_at(const math::vec2f& local, float factor);

private:
    cas::simulations::simulation _simulation;

    hex_layout _layout;
    std::unique_ptr<cas::rendering::automaton::renderer> _renderer;
    cas::rendering::automaton::config _config;
    cas::rendering::automaton::config _preview_config;

    cas::viewports::camera _camera;

    history_buffer _population_history;
    history_buffer _density_history;
    history_buffer _activity_history;
    linear_history _linear_history;

    std::string _name{ "Main" };
};

} // namespace cas::viewports
