#pragma once

#include <viewports/viewport.h>

#include <functional>

#include <span>

namespace cas::gui {

class viewport_view {
public:
    explicit viewport_view(cas::viewports::viewport& initial_viewport)
        : _viewport(initial_viewport)
    {}

    void set_active_viewport(cas::viewports::viewport& viewport) {
        _viewport = std::ref(viewport);
    }

    cas::rendering::automaton::config& config() {
        return _viewport.get().config();
    }

    cas::rendering::automaton::config& preview_config() {
        return _viewport.get().preview_config();
    }

    cas::simulations::simulation& simulation() {
        return _viewport.get().simulation();
    }

    std::span<const float> population_history() const {
        return _viewport.get().population_history().samples();
    }

    std::span<const float> density_history() const {
        return _viewport.get().density_history().samples();
    }

    std::span<const float> activity_history() const {
        return _viewport.get().activity_history().samples();
    }

    cas::viewports::viewport& get() {
        return _viewport.get();
    }

    const cas::viewports::viewport& get() const {
        return _viewport.get();
    }

private:
    std::reference_wrapper<cas::viewports::viewport> _viewport;
};

} // namespace cas