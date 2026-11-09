
#pragma once

#include <automata/automaton.h>

#include <cstdint>
#include <limits>
#include <span>
#include <vector>

namespace cas::viewports {

class linear_history {
public:
    static constexpr int max_generations = 1000;

    linear_history() = default;

    void record(const automata::automaton& automaton);

    void clear() noexcept;

    [[nodiscard]]
    int width() const noexcept;

    [[nodiscard]]
    int row_count() const noexcept;

    [[nodiscard]]
    bool empty() const noexcept;

    void copy_to(std::span<std::uint8_t> destination) const;

    [[nodiscard]]
    std::vector<std::uint8_t> chronological_data() const;

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

    void synchronize(const automata::automaton& automaton);
};

} // namespace cas::viewports
