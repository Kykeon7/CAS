#pragma once

#include <array>
#include <string_view>
#include <vector>

#include <automata/cell_state.h>
#include <automata/pattern.h>
#include <automata/topology.h>

namespace cas::automata {
class automaton;
}

namespace cas::automata::experiments {

/// @brief Identifies a reproducible, predefined automaton configuration.
enum class id {
    gosper_gun,
    glider_collision,
    highlife_replicator,
    spiral_single,
    spiral_four,
    spiral_eight,
    spiral_multistate
};

/// @brief Metadata and fixed dimensions for a reproducible experiment.
struct descriptor {
    id identifier;
    std::string_view name;
    std::string_view description;
    std::string_view rule;
    int width;
    int height;
    boundary_type boundary;
    bool hexagonal;
};

inline constexpr std::array<descriptor, 7> catalogue{{
    {id::gosper_gun, "Gosper glider gun", "Periodic glider source in Conway Life; observe emitted gliders.", "B3/S23", 160, 100, boundary_type::finite, false},
    {id::glider_collision, "Two gliders", "Two initial gliders for collision exploration; not a verified gate.", "B3/S23", 96, 96, boundary_type::finite, false},
    {id::highlife_replicator, "HighLife replicator", "Six-cell replicator seed in B36/S23.", "B36/S23", 128, 128, boundary_type::finite, false},
    {id::spiral_single, "Spiral: single cell", "Single state-1 cell; inspired by Basurto 2009 Fig. 5.2.", "Spiral", 96, 96, boundary_type::finite, true},
    {id::spiral_four, "Spiral: four cells", "Four aligned state-1 cells; inspired by Fig. 5.3.", "Spiral", 96, 96, boundary_type::finite, true},
    {id::spiral_eight, "Spiral: eight cells", "Eight aligned state-1 cells; inspired by Fig. 5.4.", "Spiral", 128, 128, boundary_type::finite, true},
    {id::spiral_multistate, "Spiral: multistate seed", "Exploratory state-1/state-2 cluster; not a published logic gate.", "Spiral", 96, 96, boundary_type::finite, true}
}};

/// @brief Constructs an isolated automaton with the preset's exact initial state.
/// @throws std::runtime_error If the rule or pattern cannot be initialized.
[[nodiscard]] automaton create(id identifier);

} // namespace cas::automata::experiments
