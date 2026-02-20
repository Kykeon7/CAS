#include "window.h"

#include <stdexcept>

namespace cas::platform {
Window::Window(std::string_view title, int width, int height) {
	m_handle = SDL_CreateWindow(title.data(), width, height, 0);

	if (!m_handle)
		throw std::runtime_error(SDL_GetError());
}

Window::~Window() {
	if (m_handle)
		SDL_DestroyWindow(m_handle);
}

}

