
#include "automata/topology.h"

#include <utility>

#include <algorithm>
#include <ranges>

namespace cas::automata {

topology::topology(
    std::vector<std::pair<int, int>> directions,
    boundary_type boundary
)
    : _directions(std::move(directions))
    , _boundary(boundary)
{
}

math::vec2i topology::normalize_dimensions(
    int width,
    int height
) const noexcept {
    if (is_linear())
        return { width, 1 };

    return { width, height };
}

int topology::neighbor_count() const noexcept {
    return static_cast<int>(_directions.size());
}

void topology::build(int width, int height) {
    if (width <= 0 || height <= 0) {
        _neighbors.clear();
        return;
    }

    const auto cell_count =
        static_cast<std::size_t>(width) *
        static_cast<std::size_t>(height);

    _neighbors.assign(
        cell_count,
        std::vector<int>(_directions.size(), -1)
    );

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int index = y * width + x;
            auto& cell_neighbors = _neighbors[index];

            for (int n = 0; n < neighbor_count(); ++n) {
                const auto& [dx, dy] = _directions[n];

                const int nx = x + dx;
                const int ny = y + dy;

                if (_boundary == boundary_type::finite) {
                    if (
                        nx < 0 || nx >= width ||
                        ny < 0 || ny >= height
                    ) {
                        continue;
                    }

                    cell_neighbors[n] = ny * width + nx;
                    continue;
                }

                const int wrapped_x = (nx + width) % width;
                const int wrapped_y = (ny + height) % height;

                cell_neighbors[n] =
                    wrapped_y * width + wrapped_x;
            }
        }
    }
}

std::span<const int> topology::neighbors(
    int index
) const noexcept {
    if (index < 0 ||
        static_cast<std::size_t>(index) >= _neighbors.size()) {
        return {};
    }

    return _neighbors[index];
}

const std::vector<std::pair<int, int>>&
topology::directions() const noexcept {
    return _directions;
}

void topology::set_boundary(boundary_type boundary) noexcept {
    _boundary = boundary;
}

boundary_type topology::boundary() const noexcept {
    return _boundary;
}

bool topology::is_linear() const noexcept {
    if (_directions.size() != 2)
        return false;

    const auto& first = _directions[0];
    const auto& second = _directions[1];

    return (
        (first.first == -1 && first.second == 0 &&
            second.first == 1 && second.second == 0) ||
        (first.first == 1 && first.second == 0 &&
            second.first == -1 && second.second == 0)
        );
}

} // namespace cas::automata
