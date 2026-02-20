#pragma once

#include <SDL3/SDL.h>
#include <array>

namespace cas {

class Input {
public:
	static Input& instance();

	void update();

	bool is_key_held(SDL_Scancode key) const;
	bool is_key_just_pressed(SDL_Scancode key) const;
	bool is_key_just_released(SDL_Scancode key) const;

private:
	Input() = default;
	~Input() = default;

	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

private:
	std::array<bool, SDL_SCANCODE_COUNT> m_prev_keys{};
	std::array<bool, SDL_SCANCODE_COUNT> m_curr_keys{};
};

} // namespace cas