#pragma once

#include <SDL3/SDL_gpu.h>

#include <platform/gpu_device.h>

namespace cas::platform {

/** Manages the lifecycle of a GPU frame and its associated resources. */
class gpu_frame {
public:
    /** Creates a frame manager associated with a GPU device. */
    explicit gpu_frame(gpu_device& device) noexcept;

    /** Cancels any unfinished frame. */
    ~gpu_frame();

    gpu_frame(const gpu_frame&) = delete;
    gpu_frame& operator=(const gpu_frame&) = delete;

    gpu_frame(gpu_frame&&) = delete;
    gpu_frame& operator=(gpu_frame&&) = delete;

    /**
     * Acquires a command buffer and the window swapchain texture.
     *
     * @throws std::logic_error If another frame is already active.
     * @throws std::runtime_error If command buffer acquisition or swapchain
     *         texture acquisition fails.
     */
    void begin_frame();

    /** Returns the command buffer associated with the active frame. */
    [[nodiscard]]
    SDL_GPUCommandBuffer* command_buffer() const noexcept {
        return command_buffer_;
    }

    /** Returns whether a swapchain texture is available for the active frame. */
    [[nodiscard]]
    bool has_swapchain_texture() const noexcept {
        return swapchain_texture_ != nullptr;
    }

    /**
     * Begins a render pass using the acquired swapchain texture.
     *
     * @param clear_color Color used to clear the render target.
     * @return The active render pass, or nullptr if no swapchain texture
     *         is available.
     * @throws std::logic_error If no frame is active or another render pass
     *         is already active.
     * @throws std::runtime_error If render pass creation fails.
     */
    [[nodiscard]]
    SDL_GPURenderPass* begin_render_pass(
        SDL_FColor clear_color = { 0.08f, 0.08f, 0.10f, 1.0f }
    );

    /**
     * Ends the active render pass and submits the command buffer.
     *
     * @throws std::logic_error If no frame is active.
     * @throws std::runtime_error If command buffer submission fails.
     */
    void end_frame();

    /**
     * Ends any active render pass and releases the command buffer.
     *
     * If a swapchain texture has been acquired, the command buffer is submitted
     * instead of cancelled, as SDL does not allow cancelling it in that state.
     */
    void cancel_frame() noexcept;

private:
    gpu_device& device_;

    SDL_GPUCommandBuffer* command_buffer_ = nullptr;
    SDL_GPUTexture* swapchain_texture_ = nullptr;
    SDL_GPURenderPass* render_pass_ = nullptr;
};

} // namespace cas::platform