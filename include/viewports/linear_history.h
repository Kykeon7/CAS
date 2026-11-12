
#pragma once

#include <automata/automaton.h>

#include <cstdint>
#include <limits>
#include <span>
#include <vector>

namespace cas::viewports {

/**
 * Stores the generation history of a linear cellular automaton.
 *
 * Each generation is stored as a row of cell states in a fixed-capacity
 * circular buffer. The history is exposed in chronological order and
 * can be exported or restored from a contiguous sequence of cell states.
 *
 * The history is synchronized with the automaton's configuration
 * revision. Changes to the automaton's configuration invalidate
 * previously recorded generations.
 */
class linear_history {
public:
    /** Maximum number of generations retained in the history. */
    static constexpr int max_generations = 1000;

    linear_history() = default;
    ~linear_history() = default;

    linear_history(const linear_history&) = default;
    linear_history& operator=(const linear_history&) = default;

    linear_history(linear_history&&) = default;
    linear_history& operator=(linear_history&&) = default;

    /**
     * Records the automaton's current generation.
     *
     * Only linear automata are supported. If the automaton is not
     * linear, the history is cleared. Configuration changes reset
     * the existing history before recording the current generation.
     *
     * A generation is recorded at most once for each generation count.
     *
     * @param automaton Automaton whose current state is recorded.
     */
    void record(const automata::automaton& automaton);

    /**
     * Removes all recorded generations and resets the history state.
     */
    void clear() noexcept;

    /**
     * Returns the width of each recorded generation.
     *
     * @return Number of cells per generation, or zero if no width
     *         is currently configured.
     */
    [[nodiscard]]
    int width() const noexcept;

    /**
     * Returns the number of generations currently stored.
     *
     * @return Number of recorded generations.
     */
    [[nodiscard]]
    int row_count() const noexcept;

    /**
     * Checks whether the history contains any recorded generations.
     *
     * @return true if the history is empty; otherwise, false.
     */
    [[nodiscard]]
    bool empty() const noexcept;

    /**
     * Copies the recorded generations into a contiguous destination.
     *
     * Generations are copied in chronological order, from oldest
     * to newest. Each generation occupies width() consecutive elements.
     *
     * If the destination is smaller than the required number of elements,
     * no data is copied.
     *
     * @param destination Destination span for the chronological data.
     */
    void copy_to(std::span<std::uint8_t> destination) const;

    /**
     * Returns all recorded generations in chronological order.
     *
     * The returned vector contains row_count() * width() elements,
     * with each generation stored as a contiguous row.
     *
     * @return Chronological sequence of recorded cell states.
     */
    [[nodiscard]]
    std::vector<std::uint8_t> chronological_data() const;

    /**
     * Restores the history from a chronological sequence of cell states.
     *
     * The supplied data must contain complete generations whose width
     * matches the automaton. The automaton must be linear and have
     * valid current-state storage. The number of generations must not
     * exceed max_generations.
     *
     * On success, the history is associated with the automaton's current
     * configuration revision, and its current generation is considered
     * already recorded.
     *
     * @param automaton Linear automaton associated with the restored history.
     * @param data Chronological sequence of cell states to restore.
     * @return true if the history was successfully restored; otherwise,
     *         false if the automaton or supplied data is incompatible.
     */
    [[nodiscard]]
    bool restore(
        const automata::automaton& automaton,
        std::span<const std::uint8_t> data
    );

private:
    int _width = 0;

    std::vector<std::uint8_t> _data;

    int _head = 0;
    int _count = 0;

    std::uint64_t _revision =
        std::numeric_limits<std::uint64_t>::max();

    int _last_recorded_generation = -1;

    /** Resets the history when the automaton configuration changes. */
    void synchronize(const automata::automaton& automaton);
};

} // namespace cas::viewports
    