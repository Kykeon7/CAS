#pragma once

#include "cell_state.h"
#include "grid_storage.h"
#include "rules/rule.h"
#include "topology.h"
#include <cstdint>
#include <memory>
#include <vector>

namespace cas::automata {

class automaton {
public:
    automaton(
        int width,
        int height,
        topology topology,
        std::unique_ptr<rules::rule> rule
    );

    automaton();

    void step();

    void randomize(std::uint32_t seed);

    void randomize(
        std::span<const std::uint32_t> percentages,
        std::uint32_t seed
    );

    bool in_bounds(int x, int y) const;

    cell_state get(int x, int y) const;
    void set(int x, int y, cell_state state);

    void resize(int width, int height);

    void restore_state(
        std::span<const cell_state> cells,
        int generation_count
    );

    int width() const noexcept;
    int height() const noexcept;

    void set_rule(std::unique_ptr<rules::rule> rule);
    void set_topology(const topology& topology);
    void set_boundary(boundary_type boundary);
    void set_random_seed(std::uint32_t seed) noexcept;

    void clean();

    const rules::rule& rule() const noexcept;
    
    const topology& topology() const noexcept;

    const grid_storage& storage() const noexcept;

    int state_count(cell_state state) const noexcept;
    int generation_count() const noexcept;

    std::uint64_t configuration_revision() const noexcept;

private:
    void invalidate_configuration() noexcept;
    void reset_history();
    void clear_state();

private:
    grid_storage _storage;
    automata::topology _topology;
    std::unique_ptr<rules::rule> _rule;

    int _requested_width;
    int _requested_height;

    std::vector<int> _state_counts;
    int _generation_count = 0;

    std::vector<std::vector<cell_state>> _history;
    std::vector<cell_state> _memory_summary;
    bool _history_valid = false;

    std::uint64_t _configuration_revision = 0;

    std::uint32_t _random_seed = 5489;
};

} // namespace cas::automata