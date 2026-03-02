#include "core/camera.h"

namespace cas {

void Camera::set_position(const Vec2& pos) {
    m_position = pos;
}

void Camera::move(const Vec2& delta) {
    m_position += delta;
}

const Vec2& Camera::position() const {
    return m_position;
}

void Camera::set_zoom(float zoom) {
    m_zoom = zoom;
}

float Camera::zoom() const {
    return m_zoom;
}

Vec2 Camera::world_to_screen(const Vec2& world) const {
    return (world - m_position) * m_zoom;
}

Vec2 Camera::screen_to_world(const Vec2& screen) const {
    return (screen * (1.0f / m_zoom)) + m_position;
}

}