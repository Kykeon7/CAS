#pragma once

#include <string>
#include <vector>

#include <automata/patterns.h>

namespace cas::automata {

struct pattern_test_result {
    std::string name;
    bool passed = false;
    std::string detail;
};

/// @brief Runs deterministic Game of Life pattern regression tests.
/// @return Results for Block, Blinker, Glider and invalid-state rejection.
[[nodiscard]] std::vector<pattern_test_result> run_pattern_tests();

/// @brief Stamps cells with their specified states at the given grid origin.
/// @details Returns false without modifying cells if any offset is out of bounds or a state is invalid.
/// @param target Automaton to modify.
/// @param source Relative pattern offsets.
/// @param x Horizontal origin.
/// @param y Vertical origin.
/// @return Whether the entire pattern was placed.
class automaton;
[[nodiscard]] bool stamp_pattern(automaton& target, const pattern& source, int x, int y);

} // namespace cas::automata
