#pragma once
#include <SDL3/SDL.h>

namespace cas::platform {
class Renderer {
public:
	explicit Renderer(SDL_Window* window);
	~Renderer();

	void clear(uint8_t r, uint8_t g, uint8_t b);
	void present();

	SDL_Renderer* native() const { return m_handle; }

private:
	SDL_Renderer* m_handle{};
};
}

