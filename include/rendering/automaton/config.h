#pragma once

#include <graphics/color.h>
#include <graphics/colors.h>
#include <math/vec2.h>

namespace cas::rendering::automaton {

/**
 * Stores the visual configuration of an automaton renderer.
 */
struct config {
    /** Background color of the automaton viewport. */
    graphics::color background_color = graphics::black;

    /** Color used to draw the cell grid. */
    graphics::color grid_color = graphics::gray;

    /** Color used to represent living cells. */
    graphics::color alive_color = graphics::green;

    /** Color used to represent persistent cells. */
    graphics::color persistent_color = graphics::yellow;

    /** Size of each cell in world-space units. */
    float cell_size = 8.0f;

    /** World-space offset applied to the automaton layout. */
    math::vec2f offset{ 0.f, 0.f };
};

} // namespace cas::rendering::automaton