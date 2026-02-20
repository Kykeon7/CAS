#pragma once

#include <string>
#include <SDL3/SDL.h>

namespace cas::platform {
class Window {
public:
	Window(std::string_view title, int width, int height);
	~Window();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	SDL_Window* native() const { return m_handle; }

private:
	SDL_Window* m_handle{};
};
}


