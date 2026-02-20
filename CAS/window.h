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

	void set_full_screen(bool enabled);

	SDL_Window* native() const { return m_handle; }
	bool is_full_screen() const { return m_is_full_screen; }

private:
	SDL_Window* m_handle{};

	bool m_is_full_screen;
};
}


