#pragma once

#include <automata/topology.h>

namespace cas::automata::topologies {

/// @brief Returns a finite hexagonal topology.
/// @return A topology with six hexagonal neighbor directions.
[[nodiscard]] inline topology hex_finite() {
    return topology({
        { 1,  0 },
        {-1,  0 },
        { 0,  1 },
        { 0, -1 },
        { 1, -1 },
        {-1,  1 }
    }, boundary_type::finite);
}

/// @brief Returns a finite linear topology.
/// @return A topology with two horizontal neighbor directions.
[[nodiscard]] inline topology linear_finite() {
    return topology({
        {-1, 0},
        { 1, 0 }
    }, boundary_type::finite);
}

/// @brief Returns a toroidal linear topology.
/// @return A topology with two horizontal neighbor directions
///         and wraparound boundaries.
[[nodiscard]] inline topology linear_toroidal() {
    return topology({
        {-1, 0},
        { 1, 0 }
    }, boundary_type::toroidal);
}

} // namespace cas::automata::topologies