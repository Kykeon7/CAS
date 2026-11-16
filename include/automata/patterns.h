#pragma once

#include <array>
#include <cctype>
#include <string_view>
#include <vector>

#include <automata/pattern.h>
#include <automata/topology.h>

namespace cas::automata::patterns {

enum class id {
    block, blinker, glider, beehive_still, loaf, boat, tub,
    toad, beacon, pulsar, pentadecathlon, lightweight_spaceship,
    gosper_glider_gun, r_pentomino, acorn, diehard,
    highlife_replicator, highlife_glider,
    seeds_pair, seeds_line, seeds_cross,
    spiral_single, spiral_pair, spiral_ring, spiral_cluster,
    beehive_single, beehive_pair, beehive_ring, beehive_cluster
};

enum class neighborhood { moore, hexagonal };

struct descriptor {
    id identifier;
    std::string_view name;
    std::string_view rule;
    std::string_view description;
    neighborhood required_topology;
    std::string_view rle;
};

// RLE uses o for state 1 and A for state 2. Hexagonal presets are initial seeds, not
// verified gliders, oscillators or guns for the three-state rules.
inline constexpr std::array<descriptor, 29> catalogue{{
    {id::block, "Block", "B3/S23", "Still life (period 1)", neighborhood::moore, "2o$2o!"},
    {id::blinker, "Blinker", "B3/S23", "Oscillator (period 2)", neighborhood::moore, "3o!"},
    {id::glider, "Glider", "B3/S23", "Spaceship (period 4)", neighborhood::moore, "bo$2bo$3o!"},
    {id::beehive_still, "Beehive", "B3/S23", "Still life (period 1)", neighborhood::moore, "b2o$o2bo$b2o!"},
    {id::loaf, "Loaf", "B3/S23", "Still life (period 1)", neighborhood::moore, "b2o$o2bo$bobo$2bo!"},
    {id::boat, "Boat", "B3/S23", "Still life (period 1)", neighborhood::moore, "2o$obo$bo!"},
    {id::tub, "Tub", "B3/S23", "Still life (period 1)", neighborhood::moore, "bo$obo$bo!"},
    {id::toad, "Toad", "B3/S23", "Oscillator (period 2)", neighborhood::moore, "b3o$3o!"},
    {id::beacon, "Beacon", "B3/S23", "Oscillator (period 2)", neighborhood::moore, "2o2b$2o2b$2b2o$2b2o!"},
    {id::pulsar, "Pulsar", "B3/S23", "Oscillator (period 3)", neighborhood::moore,
        "2b3o3b3o$13b$o4bobo4bo$o4bobo4bo$o4bobo4bo$2b3o3b3o$13b$2b3o3b3o$o4bobo4bo$o4bobo4bo$o4bobo4bo$13b$2b3o3b3o!"},
    {id::pentadecathlon, "Pentadecathlon seed", "B3/S23", "Ten-cell row; observe evolution", neighborhood::moore, "10o!"},
    {id::lightweight_spaceship, "LWSS", "B3/S23", "Lightweight spaceship", neighborhood::moore, "bo2bo$o4b$o3bo$4o!"},
    {id::gosper_glider_gun, "Gosper Glider Gun", "B3/S23", "36-cell-wide periodic glider emitter", neighborhood::moore,
        "24bo$22bobo$12b2o6b2o12b2o$11bo3bo4b2o12b2o$2o8bo5bo3b2o$2o8bo3bob2o4bobo$10bo5bo7bo$11bo3bo$12b2o!"},
    {id::r_pentomino, "R-pentomino", "B3/S23", "Long-lived methuselah", neighborhood::moore, "b2o$2o$bo!"},
    {id::acorn, "Acorn", "B3/S23", "Long-lived methuselah", neighborhood::moore, "bo$3bo$2o2b3o!"},
    {id::diehard, "Diehard", "B3/S23", "Transient pattern", neighborhood::moore, "6bo$2o$bo3b3o!"},
    {id::highlife_replicator, "Replicator seed", "B36/S23", "HighLife six-cell replicator seed", neighborhood::moore, "3o$3o!"},
    {id::highlife_glider, "Glider", "B36/S23", "Conway glider also propagates in HighLife", neighborhood::moore, "bo$2bo$3o!"},
    {id::seeds_pair, "Two-cell seed", "B2/S", "Two adjacent live cells", neighborhood::moore, "2o!"},
    {id::seeds_line, "Three-cell seed", "B2/S", "Three adjacent live cells", neighborhood::moore, "3o!"},
    {id::seeds_cross, "Cross seed", "B2/S", "Five-cell initial configuration", neighborhood::moore, "bo$3o$bo!"},
    {id::spiral_single, "Single seed", "Spiral", "Unverified single-state-1 seed", neighborhood::hexagonal, "o!"},
    {id::spiral_pair, "Adjacent pair", "Spiral", "Unverified two-cell seed", neighborhood::hexagonal, "oA!"},
    {id::spiral_ring, "Hexagonal ring", "Spiral", "Unverified six-cell seed", neighborhood::hexagonal, "b2o$oAbo$b2o!"},
    {id::spiral_cluster, "Hexagonal cluster", "Spiral", "Unverified seven-cell seed", neighborhood::hexagonal, "bAo$oAo$bAo!"},
    {id::beehive_single, "Single seed", "Beehive", "Unverified single-state-1 seed", neighborhood::hexagonal, "o!"},
    {id::beehive_pair, "Adjacent pair", "Beehive", "Unverified two-cell seed", neighborhood::hexagonal, "oA!"},
    {id::beehive_ring, "Hexagonal ring", "Beehive", "Unverified six-cell seed", neighborhood::hexagonal, "b2o$oAbo$b2o!"},
    {id::beehive_cluster, "Hexagonal cluster", "Beehive", "Unverified seven-cell seed", neighborhood::hexagonal, "bAo$oAo$bAo!"}
}};

[[nodiscard]] inline const descriptor& describe(id identifier) {
    return catalogue[static_cast<std::size_t>(identifier)];
}

/// @brief Parses the state-1 cells of a bounded run-length-encoded preset.
[[nodiscard]] inline pattern create(id identifier) {
    const auto rle = describe(identifier).rle;
    std::vector<pattern_cell> cells;
    int x = 0;
    int y = 0;
    int count = 0;
    for (const char c : rle) {
        if (c >= '0' && c <= '9') {
            count = count * 10 + (c - '0');
            continue;
        }
        const int n = count == 0 ? 1 : count;
        count = 0;
        if (c == 'o' || c == 'A') {
            const cell_state state = c == 'o' ? 1 : 2;
            for (int i = 0; i < n; ++i)
                cells.push_back({{x++, y}, state});
        }
        else if (c == 'b')
            x += n;
        else if (c == '$') {
            x = 0;
            y += n;
        }
        else if (c == '!')
            break;
    }
    return pattern(std::move(cells));
}

/// @brief Checks the exact rule and the required neighborhood.
[[nodiscard]] inline bool supports(
    id identifier,
    const topology& current_topology,
    std::string_view current_rule
) noexcept {
    const auto& entry = describe(identifier);
    const bool topology_matches = entry.required_topology == neighborhood::moore ?
        current_topology.is_moore() :
        current_topology.neighbor_count() == 6 &&
            !current_topology.is_moore() && !current_topology.is_linear();
    return topology_matches && entry.rule == current_rule;
}

} // namespace cas::automata::patterns
