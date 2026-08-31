#pragma once

#include <core/rendering/renderer.h>
#include <automaton/automaton.h>
#include <core/graphics/color.h>
#include <core/rendering/automaton_renderers/automaton_renderer.h>
#include <core/rendering/automaton_renderers/render_config.h>

namespace cas {

class AutomatonRenderer {
public:
	virtual void render(
			platform::Renderer& r,
			const automaton& a,
			const AutomatonRenderConfig& config
	) = 0;
	virtual ~AutomatonRenderer() = default;
};

} // namespace cas