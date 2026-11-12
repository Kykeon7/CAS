#pragma once

#include <span>
#include <utility>
#include <vector>

#include <math/vec2.h>

namespace cas::automata {

/// @brief Defines the neighborhood and boundary conditions of a cellular automaton.
/// @details Stores the relative directions used to identify neighboring cells
///          and the corresponding neighbor indices for each cell.
enum class boundary_type {
    finite,
    toroidal
};

class topology {
public:
    topology() = default;

    /// @brief Constructs a topology from a set of neighbor directions.
    /// @param directions Relative offsets identifying neighboring cells.
    /// @param boundary Boundary condition applied when building the topology.
    topology(
        std::vector<std::pair<int, int>> directions,
        boundary_type boundary
    );

    topology(const topology&) = default;
    topology& operator=(const topology&) = default;

    topology(topology&&) noexcept = default;
    topology& operator=(topology&&) noexcept = default;

    ~topology() = default;

    /// @brief Adjusts grid dimensions according to the topology.
    /// @param width Requested grid width.
    /// @param height Requested grid height.
    /// @return Normalized grid dimensions.
    math::vec2i normalize_dimensions(
        int width,
        int height
    ) const noexcept;

    /// @brief Builds the neighbor indices for every cell.
    /// @param width Grid width.
    /// @param height Grid height.
    /// @details Invalid dimensions clear the existing neighbor data.
    ///          The operation throws if the grid exceeds the supported
    ///          index range.
    void build(int width, int height);

    /// @brief Returns the number of configured neighbor directions.
    /// @return Number of neighbor directions.
    int neighbor_count() const noexcept {
        return static_cast<int>(_directions.size());
    }

    /// @brief Provides access to the neighbors of a cell.
    /// @param index Linear index of the cell.
    /// @return A span containing the neighbor indices, or an empty span
    ///         if the index is outside the topology.
    std::span<const int> neighbors(int index) const noexcept;

    /// @brief Provides read-only access to the configured directions.
    /// @return Constant reference to the direction collection.
    const std::vector<std::pair<int, int>>& directions() const noexcept {
        return _directions;
    }

    /// @brief Changes the boundary condition.
    /// @param boundary New boundary condition.
    /// @details Existing neighbor indices are not rebuilt automatically.
    void set_boundary(boundary_type boundary) noexcept {
        _boundary = boundary;
    }

    /// @brief Returns the current boundary condition.
    /// @return Current boundary condition.
    boundary_type boundary() const noexcept {
        return _boundary;
    }

    /// @brief Determines whether the topology represents a linear neighborhood.
    /// @return True if the directions are the two horizontal offsets (-1, 0)
    ///         and (1, 0), regardless of their order.
    bool is_linear() const noexcept;

private:
    std::vector<std::pair<int, int>> _directions;
    boundary_type _boundary{ boundary_type::finite };

    std::vector<int> _neighbors;
};

} // namespace cas::automata