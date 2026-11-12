
#include <platform/gpu_frame.h>

#include <stdexcept>
#include <string>

namespace cas::platform {

gpu_frame::gpu_frame(gpu_device& device) noexcept
    : device_(device)
{
}

gpu_frame::~gpu_frame() {
    cancel_frame();
}

void gpu_frame::begin_frame() {
    if (command_buffer_)
        throw std::logic_error("A GPU frame is already active.");

    command_buffer_ =
        SDL_AcquireGPUCommandBuffer(device_.native());

    if (!command_buffer_) {
        throw std::runtime_error(
            std::string("SDL_AcquireGPUCommandBuffer failed: ") +
            SDL_GetError()
        );
    }

    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
        command_buffer_,
        device_.native_window().native(),
        &swapchain_texture_,
        nullptr,
        nullptr
    )) {
        std::string error;

        try {
            error =
                std::string(
                    "SDL_WaitAndAcquireGPUSwapchainTexture failed: "
                ) + SDL_GetError();
        }
        catch (...) {
            cancel_frame();
            throw;
        }

        cancel_frame();
        throw std::runtime_error(error);
    }
}

SDL_GPURenderPass* gpu_frame::begin_render_pass(
    SDL_FColor clear_color
) {
    if (!command_buffer_)
        throw std::logic_error("No active GPU frame.");

    if (render_pass_)
        throw std::logic_error("A GPU render pass is already active.");

    // The swapchain texture may be unavailable while minimized.
    if (!swapchain_texture_)
        return nullptr;

    SDL_GPUColorTargetInfo color_target_info{};

    color_target_info.texture = swapchain_texture_;
    color_target_info.clear_color = clear_color;
    color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
    color_target_info.store_op = SDL_GPU_STOREOP_STORE;

    render_pass_ = SDL_BeginGPURenderPass(
        command_buffer_,
        &color_target_info,
        1,
        nullptr
    );

    if (!render_pass_) {
        throw std::runtime_error(
            std::string("SDL_BeginGPURenderPass failed: ") +
            SDL_GetError()
        );
    }

    return render_pass_;
}

void gpu_frame::end_frame() {
    if (!command_buffer_)
        throw std::logic_error("No active GPU frame.");

    if (render_pass_) {
        SDL_EndGPURenderPass(render_pass_);
        render_pass_ = nullptr;
    }

    SDL_GPUCommandBuffer* command_buffer = command_buffer_;

    command_buffer_ = nullptr;
    swapchain_texture_ = nullptr;

    if (!SDL_SubmitGPUCommandBuffer(command_buffer)) {
        throw std::runtime_error(
            std::string("SDL_SubmitGPUCommandBuffer failed: ") +
            SDL_GetError()
        );
    }
}

void gpu_frame::cancel_frame() noexcept {
    if (!command_buffer_)
        return;

    if (render_pass_) {
        SDL_EndGPURenderPass(render_pass_);
        render_pass_ = nullptr;
    }

    SDL_GPUCommandBuffer* command_buffer = command_buffer_;
    const bool has_acquired_texture = swapchain_texture_ != nullptr;

    command_buffer_ = nullptr;
    swapchain_texture_ = nullptr;

    if (has_acquired_texture)
        SDL_SubmitGPUCommandBuffer(command_buffer);
    else
        SDL_CancelGPUCommandBuffer(command_buffer);
}

} // namespace cas::platform
