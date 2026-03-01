#include "platform/window.h"

#include <stdexcept>

namespace cas::platform {
	Window::Window(std::string_view title, int width, int height) {
		m_handle = SDL_CreateWindow(title.data(), width, height, SDL_WINDOW_HIGH_PIXEL_DENSITY);

		if (!m_handle)
			throw std::runtime_error(SDL_GetError());
	}

	Window::~Window() {
		if (m_handle)
			SDL_DestroyWindow(m_handle);
	}

	void Window::set_full_screen(bool enabled) {
		m_is_full_screen = enabled;
		SDL_SetWindowFullscreen(m_handle, enabled);
	}

}
