#pragma once

#include <memory>
#include <variant>

namespace cas::platform {
    class renderer;
    class gpu_device;
    class gpu_renderer;
}

namespace cas::rendering {

struct cpu_backend {
    std::unique_ptr<cas::platform::renderer> renderer;

    cpu_backend();
    ~cpu_backend();

    cpu_backend(const cpu_backend&) = delete;
    cpu_backend& operator=(const cpu_backend&) = delete;

    cpu_backend(cpu_backend&&) noexcept;
    cpu_backend& operator=(cpu_backend&&) noexcept;
};

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

using rendering_resources = std::variant<
    cpu_backend,
    gpu_backend
>;

} // namespace cas::rendering