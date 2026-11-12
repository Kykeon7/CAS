
#pragma once

#include <math/vec2.h>

namespace cas::viewports {

/**
 * Defines the world-space corners of a camera's visible area.
 */
struct camera_bounds {
    math::vec2f top_left;      ///< Top-left world-space corner.
    math::vec2f top_right;     ///< Top-right world-space corner.
    math::vec2f bottom_left;   ///< Bottom-left world-space corner.
    math::vec2f bottom_right;  ///< Bottom-right world-space corner.
};

/**
 * Represents a 2D camera for world-to-screen coordinate transformations.
 *
 * The camera defines a visible region through its position, viewport
 * dimensions, and zoom factor. Its position represents the world-space
 * point displayed at the center of the viewport.
 */
class camera {
public:
    camera() = default;
    ~camera() = default;

    camera(const camera&) = default;
    camera& operator=(const camera&) = default;

    camera(camera&&) = default;
    camera& operator=(camera&&) = default;

    /**
     * Sets the camera's world-space position.
     *
     * @param pos New camera position.
     */
    void set_position(const math::vec2f& pos);

    /**
     * Moves the camera by the specified world-space offset.
     *
     * @param delta Movement offset.
     */
    void move(const math::vec2f& delta);

    /**
     * Returns the camera's world-space position.
     *
     * @return Constant reference to the camera position.
     */
    [[nodiscard]]
    const math::vec2f& position() const;

    /**
     * Sets the camera's zoom factor.
     *
     * @param zoom New zoom factor.
     */
    void set_zoom(float zoom);

    /**
     * Returns the current zoom factor.
     *
     * @return Current zoom factor.
     */
    [[nodiscard]]
    float zoom() const;

    /**
     * Sets the dimensions of the camera's viewport.
     *
     * @param size Viewport dimensions in pixels.
     */
    void set_viewport_size(const math::vec2i& size);

    /**
     * Returns the current viewport dimensions.
     *
     * @return Constant reference to the viewport dimensions.
     */
    [[nodiscard]]
    const math::vec2i& viewport_size() const;

    /**
     * Converts world-space coordinates into screen-space coordinates.
     *
     * The camera position is mapped to the center of the viewport.
     *
     * @param world World-space position.
     * @return Corresponding screen-space position.
     */
    [[nodiscard]]
    math::vec2f world_to_screen(const math::vec2f& world) const;

    /**
     * Converts screen-space coordinates into world-space coordinates.
     *
     * @param screen Screen-space position.
     * @return Corresponding world-space position.
     */
    [[nodiscard]]
    math::vec2f screen_to_world(const math::vec2f& screen) const;

    /**
     * Calculates the four corners of the visible world-space area.
     *
     * @return World-space bounds of the camera's viewport.
     */
    [[nodiscard]]
    camera_bounds bounds() const;

private:
    math::vec2f _position{ 0.f, 0.f };
    math::vec2i _viewport_size{ 0, 0 };
    float _zoom{ 1.f };
};

} // namespace cas::viewports
