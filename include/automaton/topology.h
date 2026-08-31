#pragma once

#include <vector>
#include <utility>

namespace cas {

enum class boundary_type {
    Finite,
    Toroidal
};

class topology {
public:

    topology() = default;

    topology(std::vector<std::pair<int, int>> directions,
        boundary_type boundary)
        : directions(std::move(directions)),
        boundary(boundary) {
    }

    int neighbor_count() const {
        return static_cast<int>(directions.size());
    }

    int neighbor_index(int i, int n, int W, int H) const {
        int x = i % W;
        int y = i / W;

        int dx = directions[n].first;
        int dy = directions[n].second;

        if (boundary == boundary_type::Finite) {
            int nx = x + dx;
            int ny = y + dy;

            if (nx < 0 || nx >= W || ny < 0 || ny >= H)
                return -1;

            return ny * W + nx;
        }

        // Toroidal
        int nx = (x + dx + W) % W;
        int ny = (y + dy + H) % H;

        return ny * W + nx;
    }

private:
    std::vector<std::pair<int, int>> directions;
    boundary_type boundary;
};

} // namespace cas