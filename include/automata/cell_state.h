#pragma once

#include <cstdint>

namespace cas::automata {

/// @brief Represents the state of a cell in a cellular automaton.
/// @details Uses an unsigned 8-bit integer to represent cell states.
using cell_state = std::uint8_t;

} // namespace cas::automata