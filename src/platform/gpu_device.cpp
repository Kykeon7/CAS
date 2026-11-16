#include <platform/gpu_device.h>

#include <stdexcept>
#include <string>

#include <system/logger.h>

namespace cas::platform {

gpu_device::gpu_device(window& window)
    : window_(window)
{
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

    const char* driver = SDL_GetGPUDeviceDriver(device_);

    system::logger::instance().info(
        "Renderer",
        std::string("GPU driver: ") +
        (driver ? driver : "Unknown")
    );

    const SDL_GPUShaderFormat formats =
        SDL_GetGPUShaderFormats(device_);

    std::string supported_formats;

    if (formats & SDL_GPU_SHADERFORMAT_SPIRV)
        supported_formats += "SPIR-V, ";

    if (formats & SDL_GPU_SHADERFORMAT_DXIL)
        supported_formats += "DXIL, ";

    if (formats & SDL_GPU_SHADERFORMAT_DXBC)
        supported_formats += "DXBC, ";

    if (formats & SDL_GPU_SHADERFORMAT_MSL)
        supported_formats += "MSL, ";

    if (formats & SDL_GPU_SHADERFORMAT_METALLIB)
        supported_formats += "METALLIB, ";

    if (!supported_formats.empty())
        supported_formats.erase(supported_formats.size() - 2);
    else
        supported_formats = "None";

    system::logger::instance().info(
        "Renderer",
        "Supported shader formats: " + supported_formats
    );
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
