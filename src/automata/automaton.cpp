#include "automata/automaton.h"

#include "automata/rules/rule.h"

#include <automata/topologies.h>

#include <automata/rules/factory.h>

#include <system/profiler.h>

#include <algorithm>
#include <cstdlib>
#include <vector>
#include <array>
#include "automata/cell_stats.h"
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <random>
#include <numeric>
#include <span>

namespace cas::automata {

automaton::automaton(
    int width,
    int height,
    automata::topology topology,
    std::unique_ptr<rules::rule> rule
)
    : _storage(
        topology.normalize_dimensions(width, height).x,
        topology.normalize_dimensions(width, height).y
    )
    , _topology(std::move(topology))
    , _rule(std::move(rule))
    , _requested_width(width)
    , _requested_height(height)
    , _state_counts(_rule->state_count(), 0)
{
    _topology.build(_storage.width(), _storage.height());
    clear_state();
}

automaton::automaton()
    : automaton(
        1000,
        1000,
        topologies::hex_finite(),
        rules::spiral()
    )
{
}

void automaton::step() {
    const int memory_depth = _rule->memory_depth();

    if (memory_depth < 0) {
        throw std::logic_error(
            "Rule memory depth cannot be negative."
        );
    }

    if (memory_depth > 0 &&
        (!_history_valid ||
         _history.size() != static_cast<std::size_t>(memory_depth))) {
        reset_history();
    }

    const int cell_count =
        static_cast<int>(_storage.size());

    const int neighbor_count =
        _topology.neighbor_count();

    const auto& current = _storage.current();
    auto& next = _storage.next();

    // Build the temporally summarized configuration.
    if (memory_depth > 0) {
        const int sample_count = memory_depth + 1;
        const int majority_threshold = sample_count / 2;

        for (int index = 0; index < cell_count; ++index) {
            int alive_count = current[index] == 1;

            for (const auto& previous : _history) {
                alive_count += previous[index] == 1;
            }

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

        const auto cell_neighbors =
            _topology.neighbors(index);

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

        next[index] = state;
        ++_state_counts[state];
    }

    // Save the previous current configuration before swapping buffers.
    if (memory_depth > 0) {
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
        throw std::runtime_error("Automaton must have at least one state.");

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

    if (percentages.size() != static_cast<std::size_t>(states))
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

bool automaton::in_bounds(int q, int r) const {
    return q >= 0 &&
        q < _storage.width() &&
        r >= 0 &&
        r < _storage.height();
}

cell_state automaton::get(int x, int y) const {
    return _storage.get(
        y * _storage.width() + x
    );
}

void automaton::set(int x, int y, cell_state state) {
    if (!in_bounds(x, y))
        return;

    _storage.set(
        y * _storage.width() + x,
        state
    );

    _history_valid = false;

    invalidate_configuration();
}

void automaton::set_rule(std::unique_ptr<rules::rule> rule) {
    if (!rule)
        return;

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

void automaton::set_random_seed(std::uint32_t seed) noexcept {
    _random_seed = seed;
}

void automaton::clean() {
    clear_state();
}

void automaton::resize(int width, int height) {
    if (width < 1 || height < 1)
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

    const auto states = _rule->state_count();

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

int automaton::width() const noexcept {
    return _storage.width();
}

int automaton::height() const noexcept {
    return _storage.height();
}

const topology& automaton::topology() const noexcept {
    return _topology;
}

const grid_storage& automaton::storage() const noexcept {
    return _storage;
}

const rules::rule& automaton::rule() const noexcept {
    return *_rule;
}

int automaton::state_count(cell_state state) const noexcept {
    if (state >= _state_counts.size())
        return 0;

    return _state_counts[state];
}

int automaton::generation_count() const noexcept {
    return _generation_count;
}

std::uint64_t automaton::configuration_revision() const noexcept {
    return _configuration_revision;
}

void automaton::invalidate_configuration() noexcept {
    ++_configuration_revision;
}

void automaton::reset_history() {
    const int depth = _rule->memory_depth();

    if (depth < 0) {
        throw std::logic_error(
            "Rule memory depth cannot be negative."
        );
    }

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