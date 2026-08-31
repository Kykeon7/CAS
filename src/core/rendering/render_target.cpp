#include <core/rendering/render_target.h>

namespace cas::platform {

void RenderTarget::create() {
    if (m_texture)
        SDL_DestroyTexture(m_texture);

    m_texture = SDL_CreateTexture(
        m_renderer.native(),
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_TARGET,
        m_width,
        m_height
    );

    SDL_SetTextureBlendMode(m_texture, SDL_BLENDMODE_BLEND);
}

void RenderTarget::bind() {
    SDL_SetRenderTarget(m_renderer.native(), m_texture);
}

void RenderTarget::unbind() {
    SDL_SetRenderTarget(m_renderer.native(), nullptr);
}

void RenderTarget::resize(Vec2 size) {
    int w = static_cast<int>(size.x);
    int h = static_cast<int>(size.y);

    if (w == m_width && h == m_height)
        return;

    m_width = w;
    m_height = h;
    create();
}

} // namespace cas::platform