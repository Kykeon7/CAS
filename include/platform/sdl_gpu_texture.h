#pragma once

#include <SDL3/SDL_gpu.h>

#include <math/vec2.h>
#include <platform/texture.h>

namespace cas::platform {

/**
 * @brief Implements the texture interface using SDL_GPUTexture.
 *
 * Manages the lifetime of an SDL GPU texture and stores its dimensions.
 * The texture is released through the associated GPU device.
 *
 * The wrapper owns the texture but does not own the GPU device.
 * The device must outlive this object and any texture it manages.
 */
class sdl_gpu_texture final : public texture {
public:
    /**
     * Creates an empty GPU texture wrapper.
     *
     * @param device GPU device used to manage the texture.
     */
    explicit sdl_gpu_texture(SDL_GPUDevice* device) noexcept;

    /** Releases the owned GPU texture. */
    ~sdl_gpu_texture() override;

    sdl_gpu_texture(const sdl_gpu_texture&) = delete;
    sdl_gpu_texture& operator=(const sdl_gpu_texture&) = delete;

    sdl_gpu_texture(sdl_gpu_texture&&) = delete;
    sdl_gpu_texture& operator=(sdl_gpu_texture&&) = delete;

    /**
     * Replaces the currently managed texture.
     *
     * Takes ownership of the supplied texture and releases the previously
     * managed one. If the supplied pointer is the current texture, only
     * its dimensions are updated.
     *
     * @param texture GPU texture whose ownership is transferred.
     * @param size Dimensions of the supplied texture in pixels.
     */
    void reset(
        SDL_GPUTexture* texture,
        const math::vec2i& size
    ) noexcept;

    /**
     * Returns the underlying SDL GPU texture.
     *
     * @return Non-owning pointer to the GPU texture, or nullptr if empty.
     */
    [[nodiscard]]
    SDL_GPUTexture* native() const noexcept;

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
    SDL_GPUDevice* _device;
    SDL_GPUTexture* _texture{ nullptr };
    math::vec2i _size{};
};

} // namespace cas::platform