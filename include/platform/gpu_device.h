#pragma once

#include <SDL3/SDL_gpu.h>

#include <platform/window.h>

namespace cas::platform {

/** Manages an SDL GPU device associated with a window. */
class gpu_device {
public:
    /**
     * Creates a GPU device and associates it with the specified window.
     *
     * @throws std::runtime_error If device creation or window association fails.
     */
    explicit gpu_device(window& window);

    /** Releases the window association and destroys the GPU device. */
    ~gpu_device();

    gpu_device(const gpu_device&) = delete;
    gpu_device& operator=(const gpu_device&) = delete;

    gpu_device(gpu_device&&) = delete;
    gpu_device& operator=(gpu_device&&) = delete;

    /** Returns the underlying SDL GPU device. */
    [[nodiscard]]
    SDL_GPUDevice* native() const noexcept {
        return device_;
    }

    /** Returns the window associated with this device. */
    [[nodiscard]]
    window& native_window() const noexcept {
        return window_;
    }

    /**
     * Returns the GPU device name.
     *
     * The returned string is owned by SDL and must not be modified or freed.
     * Returns nullptr if the device properties are unavailable.
     */
    [[nodiscard]]
    const char* name() const noexcept;

private:
    window& window_;
    SDL_GPUDevice* device_ = nullptr;
};

} // namespace cas::platform