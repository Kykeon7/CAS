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

class gpu_renderer {
public:
    explicit gpu_renderer(gpu_device& device);
    ~gpu_renderer();

    gpu_renderer(const gpu_renderer&) = delete;
    gpu_renderer& operator=(const gpu_renderer&) = delete;
    gpu_renderer(gpu_renderer&&) = delete;
    gpu_renderer& operator=(gpu_renderer&&) = delete;

    void begin_frame();

    [[nodiscard]] SDL_GPURenderPass* begin_render_pass(
        SDL_FColor clear_color = { 0.08f, 0.08f, 0.10f, 1.0f });
    void end_frame();
    void cancel_frame() noexcept;

    void resize(int width, int height);

    void draw(
        std::span<const std::byte> data,
        int data_width,
        int data_height,
        std::span<const std::byte> uniforms,
        graphics::color clear_color);

    void draw_pixels(
        std::span<const std::byte> data,
        int data_width,
        int data_height,
        std::span<const std::byte> uniforms,
        graphics::color clear_color);

    [[nodiscard]] ImTextureID texture_id() const noexcept;

    [[nodiscard]] gpu_frame& frame() noexcept {
        return frame_;
    }

    [[nodiscard]] const gpu_frame& frame() const noexcept {
        return frame_;
    }

    [[nodiscard]]
    sdl_gpu_texture& output_texture() noexcept {
        return output_texture_;
    }

    [[nodiscard]]
    const sdl_gpu_texture& output_texture() const noexcept {
        return output_texture_;
    }

private:
    void create_pipeline();
    void create_pixel_pipeline();
    void recreate_input_texture(int width, int height);
    void release_resources() noexcept;

    gpu_device& device_;
    gpu_frame frame_;

    SDL_GPUShader* vertex_shader_ = nullptr;
    SDL_GPUShader* fragment_shader_ = nullptr;
    SDL_GPUGraphicsPipeline* pipeline_ = nullptr;

    SDL_GPUShader* pixel_fragment_shader_ = nullptr;
    SDL_GPUGraphicsPipeline* pixel_pipeline_ = nullptr;

    SDL_GPUSampler* sampler_ = nullptr;
    SDL_GPUTexture* input_texture_ = nullptr;
    sdl_gpu_texture output_texture_;
    SDL_GPUTransferBuffer* transfer_buffer_ = nullptr;

    int viewport_width_ = 0;
    int viewport_height_ = 0;
    int input_width_ = 0;
    int input_height_ = 0;
};

} // namespace cas::platform