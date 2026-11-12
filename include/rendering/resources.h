#pragma once

#include <memory>
#include <variant>

namespace cas::platform {
    class renderer;
    class gpu_device;
    class gpu_renderer;
}

namespace cas::rendering {

/**
 * Holds the resources required by the CPU rendering backend.
 *
 * Owns the platform renderer used for CPU-based rendering.
 */
struct cpu_backend {
    std::unique_ptr<cas::platform::renderer> renderer;

    cpu_backend();
    ~cpu_backend();

    cpu_backend(const cpu_backend&) = delete;
    cpu_backend& operator=(const cpu_backend&) = delete;

    cpu_backend(cpu_backend&&) noexcept;
    cpu_backend& operator=(cpu_backend&&) noexcept;
};

/**
 * Holds the resources required by the GPU rendering backend.
 *
 * Owns the GPU device and the platform renderer used for GPU-based rendering.
 */
struct gpu_backend {
    std::unique_ptr<cas::platform::gpu_device> device;
    std::unique_ptr<cas::platform::gpu_renderer> renderer;

    gpu_backend();
    ~gpu_backend();

    gpu_backend(const gpu_backend&) = delete;
    gpu_backend& operator=(const gpu_backend&) = delete;

    gpu_backend(gpu_backend&&) noexcept;
    gpu_backend& operator=(gpu_backend&&) noexcept;
};

/**
 * Stores the resources associated with the selected rendering backend.
 *
 * Contains either a CPU backend or a GPU backend.
 */
using rendering_resources = std::variant<
    cpu_backend,
    gpu_backend
>;

} // namespace cas::rendering