#pragma once

#include <array>

#include <automata/cell_state.h>
#include <automata/cell_stats.h>
#include <automata/rules/rule.h>

namespace cas::automata::rules {

/// @brief Implements a binary cellular automaton rule.
class binary final : public rule {
public:
    /// @brief Constructs a binary rule.
    /// @param birth States that cause a dead cell to become alive.
    /// @param survive States that allow a living cell to survive.
    binary(
        std::array<bool, 9> birth,
        std::array<bool, 9> survive
    );

    binary(const binary&) = delete;
    binary& operator=(const binary&) = delete;

    binary(binary&&) = delete;
    binary& operator=(binary&&) = delete;

    ~binary() override = default;

    /// @brief Calculates the next state of a cell.
    /// @param stats Statistics describing the cell and its neighborhood.
    /// @return The resulting cell state.
    automata::cell_state next(
        const automata::cell_stats& stats
    ) const override;

    /// @brief Returns the number of states supported by this rule.
    /// @return Number of supported states.
    int state_count() const noexcept override;

    /// @brief Returns mutable access to the birth configuration.
    /// @return Reference to the birth configuration.
    std::array<bool, 9>& birth() noexcept;

    /// @brief Returns read-only access to the birth configuration.
    /// @return Constant reference to the birth configuration.
    const std::array<bool, 9>& birth() const noexcept;

    /// @brief Returns mutable access to the survival configuration.
    /// @return Reference to the survival configuration.
    std::array<bool, 9>& survive() noexcept;

    /// @brief Returns read-only access to the survival configuration.
    /// @return Constant reference to the survival configuration.
    const std::array<bool, 9>& survive() const noexcept;

private:
    std::array<bool, 9> _birth;
    std::array<bool, 9> _survive;
};

} // namespace cas::automata::rules