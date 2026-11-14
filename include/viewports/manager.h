#pragma once

#include <array>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>

#include <rendering/automaton/renderer.h>
#include <viewports/viewport.h>

namespace cas::viewports {

/**
 * Manages the application's collection of viewports.
 *
 * Maintains up to max_viewports viewports, tracks the active viewport,
 * and provides a factory for creating their associated automaton
 * renderers.
 *
 * At least one viewport is always kept alive.
 */
class manager {
public:
    using viewport_id = std::size_t;

    static constexpr viewport_id max_viewports = 5;

    /**
     * Describes a change in the active viewport.
     */
    struct active_viewport_change {
        viewport_id previous; ///< Previously active viewport identifier.
        viewport_id current;  ///< Newly active viewport identifier.
    };

    /**
     * Factory function used to create an automaton renderer for a viewport.
     *
     * The factory receives the viewport that will own the resulting
     * renderer.
     */
    using renderer_factory =
        std::function<std::unique_ptr<cas::rendering::automaton::renderer>(viewport&)>;

    /**
     * Creates a viewport manager with an initial viewport.
     *
     * The initial viewport occupies identifier 0 and becomes active.
     */
    manager();

    /**
     * Creates a new viewport in the first available slot.
     *
     * Assigns a default name to the viewport and creates its renderer
     * if a renderer factory has been configured.
     *
     * @return Identifier assigned to the new viewport.
     * @throws std::runtime_error if the maximum number of viewports
     *         has already been reached.
     */
    viewport_id create();

    /**
     * Destroys the specified viewport.
     *
     * Does nothing if the identifier does not exist or if the viewport
     * is the last remaining viewport. If the destroyed viewport was
     * active, another existing viewport becomes active.
     *
     * @param id Identifier of the viewport to destroy.
     */
    void destroy(viewport_id id);

    /**
     * Returns the viewport associated with the specified identifier.
     *
     * @param id Identifier of an existing viewport.
     * @return Reference to the requested viewport.
     *
     * @pre exists(id) must be true.
     */
    viewport& get(viewport_id id);

    /**
     * Returns the viewport associated with the specified identifier.
     *
     * @param id Identifier of an existing viewport.
     * @return Constant reference to the requested viewport.
     *
     * @pre exists(id) must be true.
     */
    const viewport& get(viewport_id id) const;

    /**
     * Returns the currently active viewport.
     *
     * @return Reference to the active viewport.
     */
    viewport& active_viewport();

    /**
     * Returns the currently active viewport.
     *
     * @return Constant reference to the active viewport.
     */
    const viewport& active_viewport() const;

    /**
     * Returns the identifier of the active viewport.
     *
     * @return Active viewport identifier.
     */
    [[nodiscard]]
    viewport_id active_id() const;

    /**
     * Sets the active viewport.
     *
     * Records the previous and new identifiers when the active viewport
     * changes. Setting the already active viewport has no effect.
     *
     * @param id Identifier of an existing viewport.
     *
     * @pre exists(id) must be true.
     */
    void set_active(viewport_id id);

    /**
     * Checks whether a viewport with the specified identifier exists.
     *
     * @param id Viewport identifier to check.
     * @return true if the identifier refers to an existing viewport;
     *         otherwise, false.
     */
    [[nodiscard]]
    bool exists(viewport_id id) const;

    /**
     * Returns the number of existing viewports.
     *
     * @return Number of active viewport slots.
     */
    [[nodiscard]]
    std::size_t count() const;

    /**
     * Returns mutable access to the viewport collection.
     *
     * @return Reference to the array containing all viewport slots.
     */
    std::array<std::optional<viewport>, max_viewports>&
        viewports();

    /**
     * Retrieves and clears the pending active viewport change.
     *
     * @return The most recently recorded change, or std::nullopt if
     *         no change is pending.
     */
    std::optional<active_viewport_change>
        consume_active_change() noexcept;

    /**
     * Returns read-only access to the viewport collection.
     *
     * @return Constant reference to the array containing all viewport slots.
     */
    [[nodiscard]]
    const std::array<
        std::optional<viewport>,
        max_viewports
    >& viewports() const noexcept {
        return _viewports;
    }

    /**
     * Sets the renderer factory used for viewport creation.
     *
     * If the factory is valid, it is also invoked for every existing
     * viewport, replacing their current renderers.
     *
     * Passing an empty factory disables automatic renderer creation
     * for subsequently created viewports.
     *
     * @param factory Factory function used to create viewport renderers.
     */
    void set_renderer_factory(renderer_factory factory);

private:
    std::array<std::optional<viewport>, max_viewports> _viewports;

    std::optional<active_viewport_change> _active_change;

    renderer_factory _renderer_factory;

    viewport_id _active_id = 0;
};

} // namespace cas::viewports
    