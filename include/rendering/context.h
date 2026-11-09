#pragma once

#include <rendering/backend.h>
#include <rendering/resources.h>
#include <memory>

namespace cas {
namespace viewports {
    class camera;
    class viewport;
}

namespace platform {
    class window;
    class renderer;
    class gpu_device;
    class gpu_renderer;
}
}

namespace cas::rendering {

namespace automaton {
    class renderer;
}

class context {
public:
    context(
        cas::platform::window& window,
        backend backend
    );

    ~context();

    context(const context&) = delete;
    context& operator=(const context&) = delete;

    [[nodiscard]]
    backend backend() const noexcept;

    [[nodiscard]]
    bool is_cpu_backend() const noexcept;

    [[nodiscard]]
    bool is_gpu_backend() const noexcept;

    void initialize(cas::viewports::camera& initial_camera);

    [[nodiscard]]
    bool initialized() const noexcept;

    [[nodiscard]]
    cas::platform::renderer* cpu_renderer() noexcept;

    [[nodiscard]]
    cas::platform::gpu_device* gpu_device() noexcept;

    [[nodiscard]]
    cas::platform::gpu_renderer* gpu_renderer() noexcept;

    [[nodiscard]]
    std::unique_ptr<cas::rendering::automaton::renderer>
    create_automaton_renderer(cas::viewports::viewport& viewport);

private:
    cas::platform::window& _window;
    rendering_resources _resources;
    rendering::backend _backend;
    bool _initialized = false;
};

} // namespace cas::rendering
