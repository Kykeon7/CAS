#include <rendering/context.h>
#include <platform/window.h>
#include <viewports/camera.h>
#include <platform/renderer.h>
#include <platform/gpu_renderer.h>
#include <rendering/automaton/hex_renderer.h>
#include <rendering/automaton/gpu_renderer.h>
#include <viewports/viewport.h>

#include <platform/gpu_device.h>

#include <stdexcept>
#include <string>
#include <utility>

namespace cas::rendering {

context::context(
    cas::platform::window& window,
    rendering::backend backend
)
    : _window(window),
      _resources(
          backend == rendering::backend::cpu
              ? rendering_resources{
                    std::in_place_type<cpu_backend>
                }
              : rendering_resources{
                    std::in_place_type<gpu_backend>
                }
      ),
      _backend(backend)
{
}

context::~context() = default;

backend context::backend() const noexcept {
    return _backend;
}

bool context::is_cpu_backend() const noexcept {
    return _backend == backend::cpu;
}

bool context::is_gpu_backend() const noexcept {
    return _backend == backend::gpu;
}

void context::initialize(cas::viewports::camera& initial_camera) {
    if (_initialized) {
        throw std::logic_error(
            "Rendering context is already initialized."
        );
    }

    if (is_cpu_backend()) {
        auto& resources = std::get<cpu_backend>(_resources);

        resources.renderer = std::make_unique<cas::platform::renderer>(
            _window,
            initial_camera
        );
    } 
    else {
        auto& resources = std::get<gpu_backend>(_resources);

        resources.device = std::make_unique<cas::platform::gpu_device>(
            _window
        );

        resources.renderer = std::make_unique<cas::platform::gpu_renderer>(
            *resources.device
        );
    }

    _initialized = true;
}

bool context::initialized() const noexcept {
    return _initialized;
}

cas::platform::renderer* context::cpu_renderer() noexcept {
    auto* resources = std::get_if<cpu_backend>(&_resources);

    return resources ? resources->renderer.get() : nullptr;
}

cas::platform::gpu_device* context::gpu_device() noexcept {
    auto* resources = std::get_if<gpu_backend>(&_resources);

    return resources ? resources->device.get() : nullptr;
}

cas::platform::gpu_renderer* context::gpu_renderer() noexcept {
    auto* resources = std::get_if<gpu_backend>(&_resources);

    return resources ? resources->renderer.get() : nullptr;
}

std::unique_ptr <cas::rendering::automaton::renderer>
context::create_automaton_renderer(cas::viewports::viewport& viewport) {
    if (!_initialized) {
        throw std::logic_error(
            "Cannot create an automaton renderer before initialization."
        );
    }

    if (is_cpu_backend()) {
        return std::make_unique<cas::rendering::automaton::hex_renderer>(
            *std::get<cpu_backend>(_resources).renderer,
            viewport.layout()
        );
    }

    return std::make_unique<cas::rendering::automaton::gpu_renderer>(
        *std::get<gpu_backend>(_resources).renderer
    );
}

} // namespace cas
