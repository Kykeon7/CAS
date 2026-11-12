#pragma once

#include <SDL3/SDL.h>

#include <cstdint>
#include <functional>
#include <span>

#include <graphics/color.h>
#include <graphics/colors.h>
#include <math/vec2.h>
#include <math/vec4.h>
#include <platform/window.h>
#include <viewports/camera.h>

namespace cas::platform {

/**
 * @brief Provides 2D rendering operations using SDL_Renderer.
 *
 * Supports primitive shapes, custom geometry, and texture rendering.
 * World-space coordinates are transformed into screen-space coordinates
 * using the associated camera.
 *
 * The renderer does not own the window or camera passed to its constructor.
 * Both objects must remain valid while they are being used by the renderer.
 */
class renderer {
public:
    /**
     * @brief Creates a renderer associated with a window and camera.
     *
     * @param window_ Window whose contents will be rendered.
     * @param camera_ Camera used to transform world coordinates.
     * @throws std::runtime_error If SDL fails to create the renderer.
     */
    explicit renderer(
        const platform::window& window_,
        cas::viewports::camera& camera_
    );

    /** Releases the SDL renderer. */
    ~renderer();

    renderer(const renderer&) = delete;
    renderer& operator=(const renderer&) = delete;

    /**
     * Transfers ownership of the underlying SDL renderer.
     *
     * @param other Renderer whose resources are transferred.
     */
    renderer(renderer&& other) noexcept;

    /**
     * Releases the current SDL renderer and takes ownership of another.
     *
     * @param other Renderer whose resources are transferred.
     * @return Reference to this renderer.
     */
    renderer& operator=(renderer&& other) noexcept;

    /**
     * Sets the drawing color.
     *
     * @param color New drawing color.
     */
    void set_draw_color(const graphics::color& color);

    /**
     * Sets the drawing color using 8-bit RGBA components.
     *
     * @param r Red component.
     * @param g Green component.
     * @param b Blue component.
     * @param a Alpha component.
     */
    void set_draw_color(
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a = 255
    );

    /**
     * Clears the rendering target using the specified color.
     *
     * @param color Color used to clear the target.
     */
    void clear(const graphics::color& color);

    /**
     * Clears the rendering target using 8-bit RGBA components.
     *
     * @param r Red component.
     * @param g Green component.
     * @param b Blue component.
     * @param a Alpha component.
     */
    void clear(
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a = 255
    );

    /** Presents the rendered frame to the window. */
    void present();

    /**
     * Draws the outline of a rectangle in world coordinates.
     *
     * @param pos Top-left position of the rectangle.
     * @param size Width and height of the rectangle.
     */
    void draw_rect(
        const math::vec2f& pos,
        const math::vec2f& size
    );

    /**
     * Draws a filled rectangle in world coordinates.
     *
     * @param pos Top-left position of the rectangle.
     * @param size Width and height of the rectangle.
     */
    void fill_rect(
        const math::vec2f& pos,
        const math::vec2f& size
    );

    /**
     * Draws the outline of a square in world coordinates.
     *
     * @param center Center position of the square.
     * @param size Length of each side.
     */
    void draw_square(
        const math::vec2f& center,
        float size
    );

    /**
     * Draws a filled square in world coordinates.
     *
     * @param center Center position of the square.
     * @param size Length of each side.
     */
    void fill_square(
        const math::vec2f& center,
        float size
    );

    /**
     * Draws the outline of a regular hexagon in world coordinates.
     *
     * @param center Center position of the hexagon.
     * @param size Distance from the center to each vertex.
     */
    void draw_hex(
        const math::vec2f& center,
        float size
    );

    /**
     * Draws a filled regular hexagon in world coordinates.
     *
     * @param center Center position of the hexagon.
     * @param size Distance from the center to each vertex.
     */
    void fill_hex(
        const math::vec2f& center,
        float size
    );

    /**
     * Draws custom geometry.
     *
     * @param vertices Vertex data.
     * @param indices Indices defining the geometry's triangles.
     * @throws std::length_error If the number of vertices or indices exceeds
     *         the maximum value representable by int.
     */
    void draw_geometry(
        std::span<const SDL_Vertex> vertices,
        std::span<const int> indices
    );

    /**
     * Draws a texture inside the specified destination rectangle.
     *
     * @param texture Texture to render.
     * @param destination Destination rectangle in screen coordinates.
     */
    void draw_texture(
        SDL_Texture* texture,
        const math::vec4& destination
    );

    /**
     * Draws a texture using its native dimensions.
     *
     * @param texture Texture to render.
     */
    void draw_texture(
        SDL_Texture* texture
    );

    /**
     * Returns the underlying SDL renderer.
     *
     * @return Non-owning pointer to the SDL renderer.
     */
    [[nodiscard]]
    SDL_Renderer* native() const {
        return _handle;
    }

    /**
     * Changes the camera used for world-to-screen transformations.
     *
     * @param camera Camera to associate with the renderer.
     */
    void set_camera(cas::viewports::camera& camera) {
        _camera = camera;
    }

    /**
     * Converts a world-space position into screen-space coordinates.
     *
     * @param world Position in world coordinates.
     * @return Corresponding position in screen coordinates.
     */
    [[nodiscard]]
    math::vec2f world_to_screen(
        const math::vec2f& world
    ) const;

    /**
     * Returns the zoom factor of the associated camera.
     *
     * @return Current camera zoom factor.
     */
    [[nodiscard]]
    float zoom() const;

private:
    SDL_Renderer* _handle{};

    std::reference_wrapper<cas::viewports::camera> _camera;

    graphics::color _draw_color = graphics::white;
};

} // namespace cas::platform