
#pragma once

#include <cmath>
#include <utility>

#include <math/vec2.h>

namespace cas::viewports {

/**
 * Converts between axial hexagonal coordinates and world-space positions.
 *
 * Uses a pointy-top hexagonal layout, where each cell is identified
 * by axial coordinates (q, r). The layout supports configurable cell
 * size and world-space offsets.
 */
struct hex_layout {

    /**
     * Converts axial hexagonal coordinates into world-space coordinates.
     *
     * @param q Axial column coordinate.
     * @param r Axial row coordinate.
     * @param size Hexagon size in world-space units.
     * @param offset_x Horizontal world-space offset.
     * @param offset_y Vertical world-space offset.
     * @return World-space position corresponding to the hexagonal cell.
     */
    [[nodiscard]]
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

    /**
     * Converts world-space coordinates into the nearest axial hexagonal
     * coordinates.
     *
     * Uses fractional axial coordinates followed by cube-coordinate
     * rounding to determine the nearest hexagonal cell.
     *
     * @param world_x Horizontal world-space coordinate.
     * @param world_y Vertical world-space coordinate.
     * @param size Hexagon size in world-space units.
     * @param offset_x Horizontal world-space offset.
     * @param offset_y Vertical world-space offset.
     * @return Axial coordinates (q, r) of the nearest hexagonal cell.
     */
    [[nodiscard]]
    std::pair<int, int> world_to_hex(
        float world_x,
        float world_y,
        float size = 8.f,
        float offset_x = 0.f,
        float offset_y = 0.f
    ) const {
        const float sqrt3 = 1.732050807f;

        float wx = world_x - offset_x;
        float wy = world_y - offset_y;

        float q = (sqrt3 / 3.0f * wx - 1.0f / 3.0f * wy) / size;
        float r = (2.0f / 3.0f * wy) / size;

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
