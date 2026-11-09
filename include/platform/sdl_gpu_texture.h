
#pragma once

#include <SDL3/SDL_gpu.h>

#include <math/vec2.h>
#include <platform/texture.h>

namespace cas::platform {

class sdl_gpu_texture final : public texture {
public:
    explicit sdl_gpu_texture(SDL_GPUDevice* device) noexcept;
    ~sdl_gpu_texture() override;

    sdl_gpu_texture(const sdl_gpu_texture&) = delete;
    sdl_gpu_texture& operator=(const sdl_gpu_texture&) = delete;
    sdl_gpu_texture(sdl_gpu_texture&&) = delete;
    sdl_gpu_texture& operator=(sdl_gpu_texture&&) = delete;

    // Adopta la propiedad de la textura recibida.
    void reset(
        SDL_GPUTexture* texture,
        const math::vec2i& size
    ) noexcept;

    [[nodiscard]]
    SDL_GPUTexture* native() const noexcept;

    [[nodiscard]]
    int width() const noexcept override;

    [[nodiscard]]
    int height() const noexcept override;

    [[nodiscard]]
    ImTextureID imgui_id() const noexcept override;

private:
    SDL_GPUDevice* _device;
    SDL_GPUTexture* _texture{ nullptr };
    math::vec2i _size{};
};

} // namespace cas::platform
