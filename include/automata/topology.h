#pragma once

#include <array>
#include <utility>
#include <vector>
#include <span>
#include <math/vec2.h>

namespace cas::automata {

enum class boundary_type {
    finite,
    toroidal
};

class topology {
public:
    topology() = default;

    topology(
        std::vector<std::pair<int, int>> directions,
        boundary_type boundary
    );

    math::vec2i normalize_dimensions(
        int width,
        int height
    ) const noexcept;

    void build(int width, int height);

    int neighbor_count() const noexcept;

    std::span<const int> neighbors(int index) const noexcept;

    const std::vector<std::pair<int, int>>& directions() const noexcept;

    void set_boundary(boundary_type boundary) noexcept;
    boundary_type boundary() const noexcept;

    bool is_linear() const noexcept;

private:
    std::vector<std::pair<int, int>> _directions;
    boundary_type _boundary{ boundary_type::finite };

    std::vector<std::vector<int>> _neighbors;
};

} // namespace cas::automata