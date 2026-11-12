#pragma once

#include <automata/cell_state.h>

namespace cas::automata {

/// @brief Stores the state statistics of a cell and its neighborhood.
/// @details The center cell and all valid neighboring cells are included
///          in the state counters. Each counter represents the number
///          of cells in the corresponding state.
struct cell_stats {
    /// @brief State of the center cell.
    cell_state center{ 0 };

    /// @brief Number of cells in state 0, including the center.
    int state_0{ 0 };

    /// @brief Number of cells in state 1, including the center.
    int state_1{ 0 };

    /// @brief Number of cells in state 2, including the center.
    int state_2{ 0 };
};

} // namespace cas::automata