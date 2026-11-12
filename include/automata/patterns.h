#pragma once

#include <automata/pattern.h>

namespace cas::automata::patterns {

/// @brief Returns the Block still-life pattern.
/// @return A 2x2 block of live cells.
[[nodiscard]] inline pattern block() {
    return pattern({
        { 0, 0 },
        { 1, 0 },
        { 0, 1 },
        { 1, 1 }
    });
}

/// @brief Returns the Blinker oscillator pattern.
/// @return Three horizontally aligned live cells.
[[nodiscard]] inline pattern blinker() {
    return pattern({
        { -1, 0 },
        {  0, 0 },
        {  1, 0 }
    });
}

/// @brief Returns the Beehive still-life pattern.
/// @return A six-cell Beehive configuration.
[[nodiscard]] inline pattern beehive() {
    return pattern({
        {  1, 0 },
        {  2, 1 },
        {  1, 2 },
        {  0, 2 },
        { -1, 1 },
        {  0, 0 }
    });
}

/// @brief Returns the Glider pattern.
/// @return A five-cell configuration that moves diagonally
///         across a square cellular automaton.
[[nodiscard]] inline pattern glider() {
    return pattern({
        { 1, 0 },
        { 2, 1 },
        { 0, 2 },
        { 1, 2 },
        { 2, 2 }
    });
}

/// @brief Returns the Spiral pattern.
/// @return The cell configuration defining the Spiral pattern.
[[nodiscard]] inline pattern spiral() {
    return pattern({
        // TODO: Define the Spiral pattern.
    });
}

} // namespace cas::automata::patterns