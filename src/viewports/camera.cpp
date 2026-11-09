
#include <viewports/camera.h>

namespace cas::viewports {

void camera::set_position(const math::vec2f& pos) {
    _position = pos;
}

void camera::move(const math::vec2f& delta) {
    _position += delta;
}

const math::vec2f& camera::position() const {
    return _position;
}

void camera::set_zoom(float zoom) {
    _zoom = zoom;
}

float camera::zoom() const {
    return _zoom;
}

void camera::set_viewport_size(const math::vec2i& size) {
    _viewport_size = size;
}

const math::vec2i& camera::viewport_size() const {
    return _viewport_size;
}

math::vec2f camera::world_to_screen(const math::vec2f& world) const {
    const math::vec2f viewport_center =
        static_cast<math::vec2f>(_viewport_size) * 0.5f;

    return (world - _position) * _zoom + viewport_center;
}

math::vec2f camera::screen_to_world(const math::vec2f& screen) const {
    const math::vec2f viewport_center =
        static_cast<math::vec2f>(_viewport_size) * 0.5f;

    return (screen - viewport_center) * (1.0f / _zoom)
        + _position;
}

camera_bounds camera::bounds() const {
    const math::vec2f viewport_size =
        static_cast<math::vec2f>(_viewport_size);

    return {
        .top_left = screen_to_world({ 0.f, 0.f }),
        .top_right = screen_to_world({
            viewport_size.x, 0.f
        }),
        .bottom_left = screen_to_world({
            0.f, viewport_size.y
        }),
        .bottom_right = screen_to_world({
            viewport_size.x, viewport_size.y
        })
    };
}

} // namespace cas::viewports
