#include <platform/gpu_device.h>

#include <stdexcept>
#include <string>

namespace cas::platform {

gpu_device::gpu_device(window& window)
    : window_(window) {
    constexpr SDL_GPUShaderFormat shader_formats =
        SDL_GPU_SHADERFORMAT_SPIRV |
        SDL_GPU_SHADERFORMAT_DXIL |
        SDL_GPU_SHADERFORMAT_MSL;

    device_ = SDL_CreateGPUDevice(
        shader_formats,
        false,
        nullptr
    );

    if (!device_) {
        throw std::runtime_error(
            std::string("SDL_CreateGPUDevice failed: ") +
            SDL_GetError()
        );
    }

    if (!SDL_ClaimWindowForGPUDevice(
        device_,
        window_.native()
    )) {
        const std::string error =
            std::string("SDL_ClaimWindowForGPUDevice failed: ") +
            SDL_GetError();

        SDL_DestroyGPUDevice(device_);
        device_ = nullptr;

        throw std::runtime_error(error);
    }
}

gpu_device::~gpu_device() {
    if (!device_)
        return;

    SDL_ReleaseWindowFromGPUDevice(
        device_,
        window_.native()
    );

    SDL_DestroyGPUDevice(device_);
}

const char* gpu_device::name() const noexcept {
    if (!device_)
        return nullptr;

    const SDL_PropertiesID properties =
        SDL_GetGPUDeviceProperties(device_);

    if (properties == 0)
        return nullptr;

    return SDL_GetStringProperty(
        properties,
        "SDL.gpu.device.name",
        nullptr
    );
}

} // namespace cas::platform
