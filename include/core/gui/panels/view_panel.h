#pragma once

#include <core/gui/panels/gui_panel.h>
#include <core/math/vec2.h>
#include <core/math/vec_conv.h>
#include <automaton/automaton.h>
#include <core/rendering/renderer.h>
#include <core/rendering/automaton_renderers/automaton_renderer.h>
#include <core/rendering/automaton_renderers/render_config.h>
#include <core/rendering/render_target.h>
#include <core/graphics/color.h>
#include <core/rendering/layouts/hex_layout.h>
#include <core/input/input_keys.h>
#include <core/viewport.h>

namespace cas::panels {

class ViewportPanel : public Panel {
public:
	ViewportPanel(
		Viewport& viewport
		, platform::Renderer& renderer
		, const Input& input
	)
		: m_viewport(viewport)
		, m_renderer(renderer)
		, m_target(renderer)
		, m_input(input)
	{}

	void render() override {
		ImGui::Text("Viewport");

		ImGui::PushItemWidth(-1);

		ImGui::BeginChild("Content", ImGui::GetContentRegionAvail());

		// ===== 1. Layout =====
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 pos = ImGui::GetCursorScreenPos();

		m_viewport_size = from_imgui(size);

		// ===== 2. Render-to-texture =====

		auto& profiler = system::Profiler::instance();

		profiler.begin("render");

		m_target.resize(m_viewport_size);

		m_target.bind();
		m_viewport.get().render(m_renderer);
		m_target.unbind();

		// ===== 3. Mostrar textura =====
		ImGui::Image(
			(ImTextureID)m_target.texture(),
			size
		);

		profiler.end("render");

		// ===== 4. Input =====
		bool hovered = ImGui::IsItemHovered();

        m_viewport.get().handle_input(
            m_input,
            hovered,
            from_imgui(pos)
        );

		ImGui::EndChild();

		ImGui::PopItemWidth();

	}

	void set_viewport(Viewport& vp) {
		m_viewport = vp;
	}
	
private:
	std::reference_wrapper<Viewport> m_viewport;
	platform::Renderer& m_renderer;
	platform::RenderTarget m_target;
	const Input& m_input;
	Vec2 m_viewport_size{};
};

} // namespace cas::panel