#pragma once

#include <SDL3/SDL.h>
#include <core/rendering/renderer.h>
#include <core/math/vec2.h>

namespace cas::platform {

class RenderTarget {
public:
	RenderTarget(Renderer& r, int w = 0, int h = 0)
		: m_renderer(r), m_width(w), m_height(h)
	{}
	void create();
	void bind();
	void unbind();
	void resize(Vec2 size);

	SDL_Texture* texture() const { return m_texture; }
private:
	Renderer& m_renderer;
	SDL_Texture* m_texture{};
	int m_width;
	int m_height;
};

} // namespace cas