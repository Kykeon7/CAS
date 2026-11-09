#include "platform/sdl_texture.h"

namespace cas::platform {

sdl_texture::sdl_texture(renderer& renderer)
    : _renderer(renderer)
{
}

sdl_texture::~sdl_texture() {
    if (_texture)
        SDL_DestroyTexture(_texture);
}

bool sdl_texture::resize(const math::vec2i& size) {
    if (size.x <= 0 || size.y <= 0)
        return false;

    if (_texture && _width == size.x && _height == size.y)
        return true;

    SDL_Texture* texture = SDL_CreateTexture(
        _renderer.native(),
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        size.x,
        size.y
    );

    if (!texture)
        return false;

    if (_texture)
        SDL_DestroyTexture(_texture);

    _texture = texture;
    _width = size.x;
    _height = size.y;

    return true;
}

bool sdl_texture::update(const void* pixels, int pitch) {
    if (!_texture || !pixels || pitch <= 0)
        return false;

    return SDL_UpdateTexture(
        _texture,
        nullptr,
        pixels,
        pitch
    );
}

int sdl_texture::width() const noexcept {
    return _width;
}

int sdl_texture::height() const noexcept {
    return _height;
}

ImTextureID sdl_texture::imgui_id() const noexcept {
    return reinterpret_cast<ImTextureID>(_texture);
}

} // namespace cas::platform