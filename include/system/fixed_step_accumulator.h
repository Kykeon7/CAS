#pragma once

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace cas::system {

/// @brief Accumulates elapsed time for fixed-step updates.
/// @details Each instance maintains an independent time accumulator.
///          Complete intervals are converted into a bounded number
///          of steps. Excess complete intervals are discarded while
///          the fractional remainder is preserved.
///          This class neither measures elapsed time nor executes steps.
class fixed_step_accumulator final {
public:
    fixed_step_accumulator() = default;

    fixed_step_accumulator(const fixed_step_accumulator&) = default;
    fixed_step_accumulator& operator=(const fixed_step_accumulator&) = default;

    fixed_step_accumulator(fixed_step_accumulator&&) = default;
    fixed_step_accumulator& operator=(fixed_step_accumulator&&) = default;

    ~fixed_step_accumulator() = default;

    /// @brief Calculates the number of fixed steps to execute.
    /// @param delta_time Elapsed time in seconds.
    /// @param interval Duration of one step in seconds.
    /// @param max_steps Maximum number of steps per update.
    /// @return Number of steps to execute, between zero and max_steps.
    /// @throws std::invalid_argument If any argument is invalid.
    [[nodiscard]]
    int advance(
        double delta_time,
        double interval,
        int max_steps
    ) {
        if (!std::isfinite(delta_time) || delta_time < 0.0)
            throw std::invalid_argument(
                "Delta time must be finite and non-negative."
            );

        if (!std::isfinite(interval) || interval <= 0.0)
            throw std::invalid_argument(
                "Interval must be finite and positive."
            );

        if (max_steps <= 0)
            throw std::invalid_argument(
                "Maximum steps must be positive."
            );

        const double remaining =
            interval - _accumulated_time;

        if (delta_time < remaining) {
            _accumulated_time += delta_time;
            return 0;
        }

        // At least one complete interval has elapsed.
        const double elapsed_after_first =
            delta_time - remaining;

        const double additional_steps =
            elapsed_after_first / interval;

        const int steps =
            additional_steps >= static_cast<double>(max_steps - 1)
            ? max_steps
            : 1 + static_cast<int>(additional_steps);

        // Discard overdue complete intervals, preserving the remainder.
        _accumulated_time = std::fmod(
            elapsed_after_first,
            interval
        );

        return steps;
    }

    /// @brief Discards all accumulated time.
    void reset() noexcept {
        _accumulated_time = 0.0;
    }

private:
    double _accumulated_time = 0.0;
};

} // namespace cas::system