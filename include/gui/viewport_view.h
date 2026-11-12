
#pragma once

#include <viewports/viewport.h>

#include <functional>
#include <span>

namespace cas::gui {

/**
 * @brief Provides GUI access to the currently active viewport.
 *
 * Maintains a non-owning reference to a viewport and exposes
 * its simulation, configuration, and historical data.
 */
class viewport_view {
public:
    /**
     * @brief Constructs the adapter with an initial viewport.
     *
     * @param initial_viewport Viewport to reference.
     */
    explicit viewport_view(cas::viewports::viewport& initial_viewport)
        : _viewport(initial_viewport)
    {}

    viewport_view(const viewport_view&) = delete;
    viewport_view& operator=(const viewport_view&) = delete;
    viewport_view(viewport_view&&) = delete;
    viewport_view& operator=(viewport_view&&) = delete;

    ~viewport_view() = default;

    /**
     * @brief Changes the referenced viewport.
     *
     * @param viewport New viewport to reference.
     */
    void set_active_viewport(
        cas::viewports::viewport& viewport
    ) noexcept {
        _viewport = std::ref(viewport);
    }

    [[nodiscard]] cas::rendering::automaton::config& config() {
        return _viewport.get().config();
    }

    [[nodiscard]] const cas::rendering::automaton::config& config() const {
        return _viewport.get().config();
    }

    [[nodiscard]] cas::rendering::automaton::config& preview_config() {
        return _viewport.get().preview_config();
    }

    [[nodiscard]] const cas::rendering::automaton::config& preview_config() const {
        return _viewport.get().preview_config();
    }

    [[nodiscard]] cas::simulations::simulation& simulation() {
        return _viewport.get().simulation();
    }

    [[nodiscard]] const cas::simulations::simulation& simulation() const {
        return _viewport.get().simulation();
    }

    [[nodiscard]] std::span<const float> population_history() const {
        return _viewport.get().population_history().samples();
    }

    [[nodiscard]] std::span<const float> density_history() const {
        return _viewport.get().density_history().samples();
    }

    [[nodiscard]] std::span<const float> activity_history() const {
        return _viewport.get().activity_history().samples();
    }

    [[nodiscard]] cas::viewports::viewport& get() {
        return _viewport.get();
    }

    [[nodiscard]] const cas::viewports::viewport& get() const {
        return _viewport.get();
    }

private:
    std::reference_wrapper<cas::viewports::viewport> _viewport;
};

} // namespace cas::gui
