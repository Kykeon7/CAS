
#include <automata/topology.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

#include <math/mod.h>

namespace cas::automata {

topology::topology(
    std::vector<std::pair<int, int>> directions,
    boundary_type boundary
) :
    _directions(std::move(directions)),
    _boundary(boundary)
{
    if (_directions.size() >
        static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        throw std::length_error(
            "The number of neighbor directions exceeds the supported range."
        );
    }
}

math::vec2i topology::normalize_dimensions(
    int width,
    int height
) const noexcept {
    if (is_linear())
        return { width, 1 };

    return { width, height };
}

void topology::build(int width, int height) {
    if (width <= 0 || height <= 0) {
        _neighbors.clear();
        return;
    }

    const auto w = static_cast<std::size_t>(width);
    const auto h = static_cast<std::size_t>(height);

    if (w > std::numeric_limits<std::size_t>::max() / h)
        throw std::length_error(
            "Topology dimensions exceed the maximum storage size."
        );

    const std::size_t cell_count = w * h;

    if (cell_count >
        static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        throw std::length_error(
            "Topology dimensions exceed the supported index range."
        );
    }

    const std::size_t direction_count = _directions.size();

    if (direction_count > 0 &&
        cell_count > _neighbors.max_size() / direction_count)
    {
        throw std::length_error(
            "Topology neighbor storage exceeds the maximum size."
        );
    }

    _neighbors.assign(
        cell_count * direction_count,
        -1
    );

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            const int index = y * width + x;

            const std::size_t offset =
                static_cast<std::size_t>(index) * direction_count;

            for (int n = 0; n < neighbor_count(); ++n) {
                const auto& [dx, dy] = _directions[n];

                const std::int64_t nx =
                    static_cast<std::int64_t>(x) + dx;

                const std::int64_t ny =
                    static_cast<std::int64_t>(y) + dy;

                if (_boundary == boundary_type::finite) {
                    if (
                        nx < 0 || nx >= width ||
                        ny < 0 || ny >= height
                    ) {
                        continue;
                    }

                    _neighbors[offset + n] =
                        static_cast<int>(ny * width + nx);

                    continue;
                }

                const int wrapped_x =
                    static_cast<int>(math::mod(nx, width));

                const int wrapped_y =
                    static_cast<int>(math::mod(ny, height));

                _neighbors[offset + n] =
                    wrapped_y * width + wrapped_x;
            }
        }
    }
}

std::span<const int> topology::neighbors(
    int index
) const noexcept {
    if (index < 0 || _directions.empty())
        return {};

    const std::size_t cell_index =
        static_cast<std::size_t>(index);

    const std::size_t direction_count = _directions.size();

    if (cell_index >= _neighbors.size() / direction_count)
        return {};

    const std::size_t offset =
        cell_index * direction_count;

    return std::span<const int>(
        _neighbors.data() + offset,
        direction_count
    );
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
