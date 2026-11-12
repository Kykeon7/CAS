#pragma once

#include <SDL3/SDL.h>

#include <math/vec2.h>
#include <platform/renderer.h>
#include <platform/texture.h>

namespace cas::platform {

/**
 * @brief Implements the texture interface using SDL_Texture.
 *
 * Manages a streaming RGBA texture that can be updated with pixel data
 * and displayed through Dear ImGui.
 *
 * The texture owns its SDL_Texture resource but does not own the renderer.
 * The renderer must outlive this texture.
 */
class sdl_texture final : public texture {
public:
    /**
     * Creates an empty SDL texture wrapper.
     *
     * @param renderer Renderer used to create and manage the texture.
     */
    explicit sdl_texture(renderer& renderer);

    /** Releases the owned SDL texture. */
    ~sdl_texture() override;

    sdl_texture(const sdl_texture&) = delete;
    sdl_texture& operator=(const sdl_texture&) = delete;

    sdl_texture(sdl_texture&&) = delete;
    sdl_texture& operator=(sdl_texture&&) = delete;

    /**
     * Resizes the texture to the specified dimensions.
     *
     * If a texture with the requested dimensions already exists,
     * no resource recreation is performed.
     *
     * @param size Requested texture dimensions in pixels.
     * @return true if the texture has the requested dimensions;
     *         false if the dimensions are invalid or creation fails.
     */
    [[nodiscard]]
    bool resize(const math::vec2i& size);

    /**
     * Updates the texture contents with pixel data.
     *
     * @param pixels Pointer to the source pixel data.
     * @param pitch Number of bytes between consecutive pixel rows.
     * @return true if the texture update succeeds; false otherwise.
     */
    [[nodiscard]]
    bool update(const void* pixels, int pitch);

    /**
     * Returns the texture width in pixels.
     *
     * @return Texture width.
     */
    [[nodiscard]]
    int width() const noexcept override;

    /**
     * Returns the texture height in pixels.
     *
     * @return Texture height.
     */
    [[nodiscard]]
    int height() const noexcept override;

    /**
     * Returns the texture identifier used by Dear ImGui.
     *
     * @return ImGui texture identifier.
     */
    [[nodiscard]]
    ImTextureID imgui_id() const noexcept override;

private:
    renderer& _renderer;
    SDL_Texture* _texture{ nullptr };
    int _width{ 0 };
    int _height{ 0 };
};

} // namespace cas::platform
