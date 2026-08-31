#include <core/viewport.h>
#include <automaton/topologies.h>
#include <automaton/rules.h>
#include <core/rendering/automaton_renderers/hex_renderer.h>
#include <core/time.h>

namespace cas {

Viewport::Viewport()
	: m_renderer(std::make_unique<HexRenderer>(m_layout))
{}

void cas::Viewport::handle_input(const Input & input, bool is_hovered, Vec2 viewport_pos){
	if (!is_hovered)
		return;

	Vec2 mouse = input.mouse_position();

	Vec2 local{
		mouse.x - viewport_pos.x,
		mouse.y - viewport_pos.y
	};

	if (input.is_mouse_pressed(MouseButton::Left)) {
		Vec2 world = screen_to_world(local);

		auto [q, r] = m_layout.world_to_hex(world.x, world.y);

		m_simulation.toggle_cell(q, r);
	}

	if (input.is_mouse_held(MouseButton::Right)) {
		Vec2 delta = input.mouse_delta();

		m_camera.set_position(
			m_camera.position() - delta / m_camera.zoom()
		);
	}

	float wheel = input.mouse_wheel();

	if (wheel != 0.0f) {
		float factor = (wheel > 0) ? 1.1f : 0.9f;

		Vec2 world_before = screen_to_world(local);

		float new_zoom = std::clamp(
			m_camera.zoom() * factor,
			0.1f,
			10.0f
		);

		m_camera.set_zoom(new_zoom);

		Vec2 new_pos{
			world_before.x - local.x / m_camera.zoom(),
			world_before.y - local.y / m_camera.zoom()
		};

		m_camera.set_position(new_pos);
	}
}

void Viewport::update_sim(float dt) {
	auto& time = Time::instance();

	if (!m_simulation.is_running()) return;

	if (time.every(1 / m_simulation.speed(), 0)) {
		m_simulation.step();

		m_population_history.push_back(
			static_cast<float>(m_simulation.alive_count())
		);

		if (m_population_history.size() > 500)
			m_population_history.erase(m_population_history.begin());

		float total = static_cast<float>(m_simulation.width() * m_simulation.height());
		float density = m_simulation.alive_count() / total;

		m_density_history.push_back(density);

		if (m_population_history.size() > 1) {
			float curr = m_population_history.back();
			float prev = m_population_history[m_population_history.size() - 2];

			m_activity_history.push_back(std::abs(curr - prev));
		}
	}
}

void Viewport::render(platform::Renderer& renderer) {
	m_renderer->render(renderer, m_simulation.automaton(), m_config);
}

Vec2 Viewport::screen_to_world(const Vec2& local) const {
	return {
		local.x / m_camera.zoom() + m_camera.position().x,
		local.y / m_camera.zoom() + m_camera.position().y
	};
}

} // namespace cas