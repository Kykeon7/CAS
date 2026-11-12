
#include <platform/sdl_gpu_texture.h>

#include <cstdint>

namespace cas::platform {

sdl_gpu_texture::sdl_gpu_texture(SDL_GPUDevice* device) noexcept
    : _device(device)
{
}

sdl_gpu_texture::~sdl_gpu_texture() {
    reset(nullptr, {});
}

void sdl_gpu_texture::reset(
    SDL_GPUTexture* texture,
    const math::vec2i& size
) noexcept {
    if (_texture == texture) {
        if (_texture)
            _size = size;

        return;
    }

    if (_texture)
        SDL_ReleaseGPUTexture(_device, _texture);

    _texture = texture;

    if (_texture) {
        _size = size;
    }
    else {
        _size = {};
    }
}

SDL_GPUTexture* sdl_gpu_texture::native() const noexcept {
    return _texture;
}

int sdl_gpu_texture::width() const noexcept {
    return _size.x;
}

int sdl_gpu_texture::height() const noexcept {
    return _size.y;
}

ImTextureID sdl_gpu_texture::imgui_id() const noexcept {
    return static_cast<ImTextureID>(
        reinterpret_cast<intptr_t>(_texture)
    );
}

} // namespace cas::platform
