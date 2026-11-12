#pragma once

#include <SDL3/SDL_gpu.h>
#include <imgui.h>

#include <cstddef>
#include <span>

#include <graphics/color.h>
#include <platform/gpu_device.h>
#include <platform/gpu_frame.h>
#include <platform/sdl_gpu_texture.h>

namespace cas::platform {

/** Renders simulation data using SDL GPU pipelines. */
class gpu_renderer {
public:
    /**
     * Creates the renderer and initializes its GPU pipelines.
     *
     * @throws std::runtime_error If GPU resource initialization fails.
     */
    explicit gpu_renderer(gpu_device& device);

    /** Releases all GPU resources owned by the renderer. */
    ~gpu_renderer();

    gpu_renderer(const gpu_renderer&) = delete;
    gpu_renderer& operator=(const gpu_renderer&) = delete;

    gpu_renderer(gpu_renderer&&) = delete;
    gpu_renderer& operator=(gpu_renderer&&) = delete;

    /** Acquires the resources required to begin a frame. */
    void begin_frame();

    /**
     * Begins a render pass targeting the window swapchain.
     *
     * @param clear_color Color used to clear the render target.
     * @return The active render pass, or nullptr if no swapchain texture
     *         is available.
     */
    [[nodiscard]]
    SDL_GPURenderPass* begin_render_pass(
        SDL_FColor clear_color = { 0.08f, 0.08f, 0.10f, 1.0f }
    );

    /** Ends the active frame and submits its command buffer. */
    void end_frame();

    /** Cancels the active frame. */
    void cancel_frame() noexcept;

    /** Resizes the renderer's output texture. */
    void resize(int width, int height);

    /**
     * Renders simulation data using the standard rendering pipeline.
     *
     * @param data Simulation data.
     * @param data_width Width of the input data.
     * @param data_height Height of the input data.
     * @param uniforms Fragment shader uniform data.
     * @param clear_color Output texture clear color.
     */
    void draw(
        std::span<const std::byte> data,
        int data_width,
        int data_height,
        std::span<const std::byte> uniforms,
        graphics::color clear_color
    );

    /**
     * Renders simulation data using the pixel rendering pipeline.
     *
     * @param data Simulation data.
     * @param data_width Width of the input data.
     * @param data_height Height of the input data.
     * @param uniforms Fragment shader uniform data.
     * @param clear_color Output texture clear color.
     */
    void draw_pixels(
        std::span<const std::byte> data,
        int data_width,
        int data_height,
        std::span<const std::byte> uniforms,
        graphics::color clear_color
    );

    /** Returns the output texture identifier used by Dear ImGui. */
    [[nodiscard]]
    ImTextureID texture_id() const noexcept;

    /** Returns the frame manager. */
    [[nodiscard]]
    gpu_frame& frame() noexcept {
        return _frame;
    }

    /** Returns the frame manager. */
    [[nodiscard]]
    const gpu_frame& frame() const noexcept {
        return _frame;
    }

    /** Returns the renderer's output texture. */
    [[nodiscard]]
    sdl_gpu_texture& output_texture() noexcept {
        return _output_texture;
    }

    /** Returns the renderer's output texture. */
    [[nodiscard]]
    const sdl_gpu_texture& output_texture() const noexcept {
        return _output_texture;
    }

private:
    void create_pipeline();
    void create_pixel_pipeline();
    void recreate_input_texture(int width, int height);
    void release_resources() noexcept;

    gpu_device& _device;
    gpu_frame _frame;

    SDL_GPUShader* _vertex_shader = nullptr;
    SDL_GPUShader* _fragment_shader = nullptr;
    SDL_GPUGraphicsPipeline* _pipeline = nullptr;

    SDL_GPUShader* _pixel_fragment_shader = nullptr;
    SDL_GPUGraphicsPipeline* _pixel_pipeline = nullptr;

    SDL_GPUSampler* _sampler = nullptr;
    SDL_GPUTexture* _input_texture = nullptr;
    sdl_gpu_texture _output_texture;
    SDL_GPUTransferBuffer* _transfer_buffer = nullptr;

    int _viewport_width = 0;
    int _viewport_height = 0;
    int _input_width = 0;
    int _input_height = 0;
};

} // namespace cas::platform