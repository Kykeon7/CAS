#pragma once

#include <imgui.h>

namespace cas::platform {

/**
 * Defines the interface for textures that can be displayed by Dear ImGui.
 *
 * Provides access to texture dimensions and the identifier required
 * by ImGui to render the texture.
 */
class texture {
public:
    /** Virtual destructor for derived texture implementations. */
    virtual ~texture() = default;

    texture(const texture&) = delete;
    texture& operator=(const texture&) = delete;

    texture(texture&&) = delete;
    texture& operator=(texture&&) = delete;

    /**
     * Returns the texture width in pixels.
     *
     * @return Texture width.
     */
    [[nodiscard]]
    virtual int width() const noexcept = 0;

    /**
     * Returns the texture height in pixels.
     *
     * @return Texture height.
     */
    [[nodiscard]]
    virtual int height() const noexcept = 0;

    /**
     * Returns the texture identifier used by Dear ImGui.
     *
     * @return ImGui texture identifier.
     */
    [[nodiscard]]
    virtual ImTextureID imgui_id() const noexcept = 0;

protected:
    /** Constructs the abstract texture interface. */
    texture() = default;
};

} // namespace cas::platform