#pragma once

#include <automata/automaton.h>
#include <viewports/camera.h>
#include <math/vec2.h>
#include <rendering/automaton/renderer.h>
#include <rendering/automaton/config.h>
#include <platform/texture.h>

#include <functional>
#include <optional>
#include <cstdint>
#include <vector>
#include <limits>

namespace cas::platform {
class gpu_renderer;
}

namespace cas::rendering::automaton {

class gpu_renderer final : public renderer {
public:
    explicit gpu_renderer(
        platform::gpu_renderer& renderer
    ) noexcept;

    gpu_renderer(const gpu_renderer&) = delete;
    gpu_renderer& operator=(const gpu_renderer&) = delete;

    void resize(const math::vec2i& size) override;

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