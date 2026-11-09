#pragma once
#include <math/vec2.h>

namespace cas::viewports {

struct camera_bounds {
    math::vec2f top_left;
    math::vec2f top_right;
    math::vec2f bottom_left;
    math::vec2f bottom_right;
};

class camera {
public:
    camera() = default;

    void set_position(const math::vec2f& pos);
    void move(const math::vec2f& delta);

    const math::vec2f& position() const;

    void set_zoom(float zoom);
    float zoom() const;

    void set_viewport_size(const math::vec2i& size);
    const math::vec2i& viewport_size() const;

    math::vec2f world_to_screen(const math::vec2f& world) const;
    math::vec2f screen_to_world(const math::vec2f& screen) const;

    camera_bounds bounds() const;

private:
    math::vec2f _position{ 0.f, 0.f };
    math::vec2i _viewport_size{ 0, 0 };
    float _zoom{ 1.f };
};

} // namespace cas::viewports