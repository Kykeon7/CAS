#pragma once

#include <core/graphics/color.h>
#include <core/graphics/colors.h>
#include <core/math/vec2.h>

namespace cas {

struct AutomatonRenderConfig {
	color background_color = colors::black;
	color grid_color = colors::gray;
	color alive_color = colors::green;
	color persistent_color = colors::yellow;
	float cell_size = 8.0f;
	Vec2 offset{ 0.f, 0.f };
};

} // namespace cas
