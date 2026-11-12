#include <automata/automaton.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <numeric>
#include <random>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include <automata/cell_stats.h>
#include <automata/rules/factory.h>
#include <automata/rules/rule.h>
#include <automata/topologies.h>

namespace cas::automata {

automaton::automaton(
    int width,
    int height,
    automata::topology topology,
    std::unique_ptr<rules::rule> rule
) :
    _storage(
        topology.normalize_dimensions(width, height).x,
        topology.normalize_dimensions(width, height).y
    ),
    _topology(std::move(topology)),
    _rule(std::move(rule)),
    _requested_width(width),
    _requested_height(height) 
{
    if (!_rule)
        throw std::invalid_argument(
            "Automaton requires a valid transition rule."
        );

    if (_rule->state_count() <= 0)
        throw std::invalid_argument(
            "Rule must have at least one state."
        );

    _topology.build(_storage.width(), _storage.height());
    clear_state();
}

automaton::automaton() :
    automaton(
        1000,
        1000,
        topologies::hex_finite(),
        rules::spiral()
    ) {
}

automaton::automaton(automaton&&) = default;

automaton& automaton::operator=(automaton&&) = default;

automaton::~automaton() = default;

void automaton::step() {
    const int memory_depth = _rule->memory_depth();

    if (memory_depth < 0)
        throw std::logic_error(
            "Rule memory depth cannot be negative."
        );

    const bool requires_history =
        memory_depth > 0 &&
        (!_history_valid ||
            _history.size() != static_cast<std::size_t>(memory_depth));

    if (requires_history)
        reset_history();

    const int cell_count = static_cast<int>(_storage.size());
    const int neighbor_count = _topology.neighbor_count();

    const auto& current = _storage.current();
    auto& next = _storage.next();

    // Build the temporally summarized configuration.
    if (memory_depth > 0) {
        const int sample_count = memory_depth + 1;
        const int majority_threshold = sample_count / 2;

        for (int index = 0; index < cell_count; ++index) {
            int alive_count = current[index] == 1;

            for (const auto& previous : _history)
                alive_count += previous[index] == 1;

            _memory_summary[index] =
                alive_count > majority_threshold ? 1 : 0;
        }
    }

    const auto& source =
        memory_depth > 0 ? _memory_summary : current;

    std::fill(
        _state_counts.begin(),
        _state_counts.end(),
        0
    );

    for (int index = 0; index < cell_count; ++index) {
        const cell_state center = source[index];

        cell_stats stats{};
        stats.center = center;

        if (center == 0)
            ++stats.state_0;
        else if (center == 1)
            ++stats.state_1;
        else if (center == 2)
            ++stats.state_2;

        const auto cell_neighbors = _topology.neighbors(index);

        for (int n = 0; n < neighbor_count; ++n) {
            const int neighbor_index = cell_neighbors[n];

            if (neighbor_index < 0)
                continue;

            const cell_state state = source[neighbor_index];

            if (state == 0)
                ++stats.state_0;
            else if (state == 1)
                ++stats.state_1;
            else if (state == 2)
                ++stats.state_2;
        }

        const cell_state state = _rule->next(stats);

        if (static_cast<std::size_t>(state) >= _state_counts.size())
            throw std::logic_error(
                "Rule returned a state outside its declared state range."
            );

        next[index] = state;
        ++_state_counts[state];
    }

    if (memory_depth > 0) {
        // Preserve the previous configuration before swapping buffers.
        _history.insert(
            _history.begin(),
            current
        );

        if (_history.size() >
            static_cast<std::size_t>(memory_depth)) {
            _history.pop_back();
        }

        _history_valid = true;
    }

    _storage.swap_buffers();

    ++_generation_count;
}


void automaton::randomize(std::uint32_t seed) {
    const std::size_t states = _rule->state_count();

    if (states == 0)
        throw std::runtime_error(
            "Automaton must have at least one state."
        );

    std::vector<std::uint32_t> percentages(states, 100 / states);

    for (std::size_t i = 0; i < 100 % states; ++i)
        ++percentages[i];

    randomize(percentages, seed);
}

void automaton::randomize(
    std::span<const std::uint32_t> percentages,
    std::uint32_t seed
) {
    const int states = _rule->state_count();

    if (states <= 0)
        throw std::logic_error(
            "Rule must have at least one state."
        );

    const bool has_valid_percentage_count =
        percentages.size() == static_cast<std::size_t>(states);

    if (!has_valid_percentage_count)
        throw std::invalid_argument(
            "Percentage count must match the rule state count."
        );

    const std::uint64_t total = std::accumulate(
        percentages.begin(),
        percentages.end(),
        std::uint64_t{ 0 }
    );

    if (total != 100)
        throw std::invalid_argument(
            "Randomization percentages must sum to 100."
        );

    std::mt19937 generator(seed);

    std::discrete_distribution<int> distribution(
        percentages.begin(),
        percentages.end()
    );

    _generation_count = 0;

    std::fill(
        _state_counts.begin(),
        _state_counts.end(),
        0
    );

    const int size = static_cast<int>(_storage.size());

    for (int i = 0; i < size; ++i) {
        const cell_state state =
            static_cast<cell_state>(distribution(generator));

        _storage.set_next(i, state);
        ++_state_counts[state];
    }

    _storage.swap_buffers();

    _history_valid = false;

    invalidate_configuration();
}

bool automaton::in_bounds(int x, int y) const {
    return x >= 0 &&
        x < _storage.width() &&
        y >= 0 &&
        y < _storage.height();
}

cell_state automaton::get(int x, int y) const {
    if (!in_bounds(x, y))
        return cell_state{};

    return _storage.get(
        y * _storage.width() + x
    );
}

void automaton::set(int x, int y, cell_state state) {
    if (!in_bounds(x, y))
        return;

    const std::size_t index =
        static_cast<std::size_t>(y) * _storage.width() + x;

    const std::size_t new_state = state;

    if (new_state >= _state_counts.size())
        throw std::invalid_argument(
            "Cell state is not supported by the current rule."
        );

    const cell_state previous_state = _storage.get(index);

    if (previous_state != state) {
        --_state_counts[previous_state];
        ++_state_counts[new_state];

        _storage.set(index, state);
    }

    _history_valid = false;

    invalidate_configuration();
}

void automaton::set_rule(std::unique_ptr<rules::rule> rule) {
    if (!rule)
        return;

    if (rule->state_count() <= 0)
        throw std::invalid_argument(
            "Rule must have at least one state."
        );

    _rule = std::move(rule);

    clear_state();
}

void automaton::set_topology(
    const automata::topology& topology
) {
    _topology = topology;

    resize(_requested_width, _requested_height);
}

void automaton::set_boundary(boundary_type boundary) {
    if (_topology.boundary() == boundary)
        return;

    _topology.set_boundary(boundary);
    _topology.build(_storage.width(), _storage.height());

    invalidate_configuration();
}

void automaton::clean() {
    clear_state();
}

void automaton::resize(int width, int height) {
    const bool has_valid_dimensions = width >= 1 && height >= 1;

    if (!has_valid_dimensions)
        return;

    _requested_width = width;
    _requested_height = height;

    const math::vec2i dimensions =
        _topology.normalize_dimensions(width, height);

    _storage.resize(dimensions.x, dimensions.y);

    _topology.build(dimensions.x, dimensions.y);

    clear_state();
}

void automaton::restore_state(
    std::span<const cell_state> cells,
    int generation_count
) {
    if (generation_count < 0)
        throw std::invalid_argument(
            "Generation count cannot be negative."
        );

    if (cells.size() != _storage.size())
        throw std::invalid_argument(
            "Cell count does not match automaton dimensions."
        );

    const int states = _rule->state_count();

    if (states <= 0)
        throw std::logic_error(
            "Rule must have at least one state."
        );

    for (const cell_state state : cells) {
        if (state >= states)
            throw std::invalid_argument(
                "Cell contains a state unsupported by the rule."
            );
    }

    std::fill(
        _state_counts.begin(),
        _state_counts.end(),
        0
    );

    auto& next = _storage.next();

    std::fill(next.begin(), next.end(), cell_state{});

    for (std::size_t i = 0; i < cells.size(); ++i) {
        _storage.set(i, cells[i]);
        ++_state_counts[cells[i]];
    }

    _generation_count = generation_count;

    _history_valid = false;

    invalidate_configuration();
}

const rules::rule& automaton::rule() const noexcept {
    return *_rule;
}

int automaton::state_count(cell_state state) const noexcept {
    if (state >= _state_counts.size())
        return 0;

    return _state_counts[state];
}

void automaton::invalidate_configuration() noexcept {
    ++_configuration_revision;
}

void automaton::reset_history() {
    const int depth = _rule->memory_depth();

    if (depth < 0)
        throw std::logic_error(
            "Rule memory depth cannot be negative."
        );

    _history.assign(
        static_cast<std::size_t>(depth),
        _storage.current()
    );

    _memory_summary.resize(_storage.size());
    _history_valid = true;
}

void automaton::clear_state() {
    const std::size_t size = _storage.size();

    for (std::size_t i = 0; i < size; ++i)
        _storage.set(i, cell_state{ 0 });

    auto& next = _storage.next();

    std::fill(next.begin(), next.end(), cell_state{ 0 });

    _state_counts.assign(_rule->state_count(), 0);

    if (!_state_counts.empty())
        _state_counts[0] = static_cast<int>(size);

    _generation_count = 0;

    _history.clear();
    _memory_summary.clear();
    _history_valid = false;

    invalidate_configuration();
}

} // namespace cas::automata
