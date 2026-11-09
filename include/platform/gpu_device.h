#pragma once

#include <SDL3/SDL_gpu.h>

#include <platform/window.h>

namespace cas::platform {

class gpu_device {
public:
    explicit gpu_device(window& window);
    ~gpu_device();

    gpu_device(const gpu_device&) = delete;
    gpu_device& operator=(const gpu_device&) = delete;

    gpu_device(gpu_device&&) = delete;
    gpu_device& operator=(gpu_device&&) = delete;

    [[nodiscard]]
    SDL_GPUDevice* native() const noexcept {
        return device_;
    }

    [[nodiscard]]
    window& native_window() const noexcept {
        return window_;
    }

    [[nodiscard]]
    const char* name() const noexcept;

private:
    window& window_;
    SDL_GPUDevice* device_ = nullptr;
};

} // namespace cas::platform
