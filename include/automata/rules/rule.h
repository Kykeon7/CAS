#pragma once

#include <automata/cell_state.h>
#include <automata/cell_stats.h>

namespace cas::automata::rules {

/// @brief Abstract interface for cellular automaton transition rules.
class rule {
public:
    rule() = default;

    rule(const rule&) = delete;
    rule& operator=(const rule&) = delete;

    rule(rule&&) = delete;
    rule& operator=(rule&&) = delete;

    virtual ~rule() = default;

    /// @brief Calculates the next state of a cell.
    /// @param stats Statistics describing the cell and its neighborhood.
    /// @return A valid state index within the range declared by state_count().
    virtual automata::cell_state next(
        const automata::cell_stats& stats
    ) const = 0;

    /// @brief Returns the number of states supported by the rule.
    /// @return A positive number of states.
    virtual int state_count() const noexcept = 0;

    /// @brief Returns the number of previous generations required by the rule.
    /// @return Required history depth. Zero means no temporal memory.
    virtual int memory_depth() const noexcept {
        return 0;
    }
};

} // namespace cas::automata::rules