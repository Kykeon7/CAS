#pragma once

#include <vector>

#include <automata/cell_state.h>
#include <math/vec2.h>

namespace cas::automata {

/// @brief A relative cell position and its nonzero automaton state.
struct pattern_cell {
    math::vec2i offset;
    cell_state state = 1;
};

/// @brief A collection of relative cells with explicit states.
class pattern {
public:
    pattern() = default;

    /// @brief Constructs a state-1 pattern from relative positions.
    explicit pattern(std::vector<math::vec2i> offsets);

    /// @brief Constructs a multistate pattern.
    explicit pattern(std::vector<pattern_cell> cells);

    /// @brief Returns all cells and their states.
    [[nodiscard]] const std::vector<pattern_cell>& cells() const noexcept {
        return _cells;
    }

private:
    std::vector<pattern_cell> _cells;
};

} // namespace cas::automata
