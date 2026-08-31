#pragma once

#include <core/gui/window.h>

namespace cas{

class LayoutManager {
public:
	void add_window(std::unique_ptr<Window> window) {
		m_windows.push_back(std::move(window));
	}

	void render() {
		for (auto& w : m_windows)
			w->render();
	}

private:
	std::vector<std::unique_ptr<Window>> m_windows;
};

} // namespace cas