#pragma once

#include <cmath>
#include "math/vec2.h"

namespace cas::viewports {

struct hex_layout {

    math::vec2f hex_to_world(
        int q,
        int r,
        float size = 8.f,
        float offset_x = 0.f,
        float offset_y = 0.f
    ) const {
        const float sqrt3 = 1.732050807f;

        float x = size * sqrt3 * (q + r * 0.5f);
        float y = size * 1.5f * r;

        return { x + offset_x, y + offset_y };
    }

    std::pair<int, int> world_to_hex(
        float world_x, 
        float world_y,
        float size = 8.f,
        float offset_x = 0.f,
        float offset_y = 0.f
    ) const {
        const float sqrt3 = 1.732050807f;

        // quitar offset
        float wx = world_x - offset_x;
        float wy = world_y - offset_y;

        // axial fraccional
        float q = (sqrt3 / 3.0f * wx - 1.0f / 3.0f * wy) / size;
        float r = (2.0f / 3.0f * wy) / size;

        // cube coords
        float x = q;
        float z = r;
        float y = -x - z;

        float rx = std::round(x);
        float ry = std::round(y);
        float rz = std::round(z);

        float x_diff = std::abs(rx - x);
        float y_diff = std::abs(ry - y);
        float z_diff = std::abs(rz - z);

        if (x_diff > y_diff && x_diff > z_diff)
            rx = -ry - rz;
        else if (y_diff > z_diff)
            ry = -rx - rz;
        else
            rz = -rx - ry;

        return {
            static_cast<int>(rx),
            static_cast<int>(rz)
        };
    }
};

} // namespace cas::viewports