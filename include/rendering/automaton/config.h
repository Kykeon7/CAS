#pragma once

#include <graphics/color.h>
#include <graphics/colors.h>
#include <math/vec2.h>

namespace cas::rendering::automaton {

struct config {
    graphics::color background_color = graphics::black;
    graphics::color grid_color = graphics::gray;

    graphics::color alive_color = graphics::green;
    graphics::color persistent_color = graphics::yellow;

    float cell_size = 8.0f;
    math::vec2f offset{ 0.f, 0.f };
};

} // namespace cas::rendering::automaton
