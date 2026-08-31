#pragma once

#include <core/rendering/layouts/hex_layout.h>
#include <core/rendering/automaton_renderers/automaton_renderer.h>

namespace cas {

class HexRenderer : public AutomatonRenderer {
public:
	HexRenderer(layouts::HexLayout layout) : m_layout(layout) {}
	void render(
		platform::Renderer& r, 
		const automaton& a, 
		const AutomatonRenderConfig& config
	) override {
		r.clear(config.background_color);
		
		r.set_draw_color(config.grid_color);

		for (int y = 0; y < a.height(); y++) {
			for (int x = 0; x < a.width(); x++) {
				Vec2 pos = m_layout.hex_to_world(
					x,
					y,
					config.cell_size,
					config.offset.x,
					config.offset.y
					);
				r.draw_hex(pos, config.cell_size);
			}
		}

		r.set_draw_color(config.alive_color);

		for (int y = 0; y < a.height(); y++) {
			for (int x = 0; x < a.width(); x++) {
				if (!a.get(x, y)) continue;

				Vec2 pos = m_layout.hex_to_world(
					x,
					y,
					config.cell_size,
					config.offset.x,
					config.offset.y
				);
				r.fill_hex(pos, config.cell_size);
			}
		}
	}
	

private:
	layouts::HexLayout m_layout;
};

} // namespace cas