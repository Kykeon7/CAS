#pragma once

#include <automata/automaton.h>
#include <viewports/camera.h>
#include <math/vec2.h>
#include <rendering/automaton/config.h>
#include <platform/texture.h>
#include <viewports/linear_history.h>

#include <functional>
#include <optional>

namespace cas::rendering::automaton {

class renderer {
public:
    virtual ~renderer() = default;

    renderer() = default;

    renderer(const renderer&) = delete;
    renderer& operator=(const renderer&) = delete;

    virtual void resize(const math::vec2i& size) = 0;

    [[nodiscard]]
    virtual std::optional<std::reference_wrapper<platform::texture>> render(
        const automata::automaton& automaton,
        const viewports::linear_history& history,
        const config& config,
        const cas::viewports::camera& camera
    ) = 0;
};

} // namespace cas::rendering::automaton