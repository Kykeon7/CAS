#pragma once

#include <SDL3/SDL.h>

#include <math/vec2.h>
#include <platform/renderer.h>
#include <platform/texture.h>

namespace cas::platform {

class sdl_texture final : public texture {
public:
    explicit sdl_texture(renderer& renderer);
    ~sdl_texture() override;

    sdl_texture(const sdl_texture&) = delete;
    sdl_texture& operator=(const sdl_texture&) = delete;

    [[nodiscard]]
    bool resize(const math::vec2i& size);

    [[nodiscard]]
    bool update(const void* pixels, int pitch);

    [[nodiscard]]
    int width() const noexcept override;

    [[nodiscard]]
    int height() const noexcept override;

    [[nodiscard]]
    ImTextureID imgui_id() const noexcept override;

private:
    renderer& _renderer;
    SDL_Texture* _texture{ nullptr };
    int _width{ 0 };
    int _height{ 0 };
};

} // namespace cas::platform