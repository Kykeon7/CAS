
#pragma once

#include <SDL3/SDL_gpu.h>

#include <platform/gpu_device.h>

namespace cas::platform {

class gpu_frame {
public:
    explicit gpu_frame(gpu_device& device) noexcept;

    ~gpu_frame();

    gpu_frame(const gpu_frame&) = delete;
    gpu_frame& operator=(const gpu_frame&) = delete;

    gpu_frame(gpu_frame&&) = delete;
    gpu_frame& operator=(gpu_frame&&) = delete;

    void begin_frame();

    [[nodiscard]]
    SDL_GPUCommandBuffer* command_buffer() const noexcept {
        return command_buffer_;
    }

    [[nodiscard]]
    bool has_swapchain_texture() const noexcept {
        return swapchain_texture_ != nullptr;
    }

    [[nodiscard]]
    SDL_GPURenderPass* begin_render_pass(
        SDL_FColor clear_color = { 0.08f, 0.08f, 0.10f, 1.0f }
    );

    void end_frame();

    void cancel_frame() noexcept;

private:
    gpu_device& device_;

    SDL_GPUCommandBuffer* command_buffer_ = nullptr;
    SDL_GPUTexture* swapchain_texture_ = nullptr;
    SDL_GPURenderPass* render_pass_ = nullptr;
};

} // namespace cas::platform
