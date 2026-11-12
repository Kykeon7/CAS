#pragma once

#include <SDL3/SDL.h>

#include <array>

#include <inputs/input_keys.h>
#include <math/vec2.h>

namespace cas::inputs {

    /**
     * @brief Tracks keyboard and mouse input states.
     *
     * Maintains current and previous input states to detect
     * held, pressed, and released keys and mouse buttons.
     */
    class input {
    public:
        input() = default;

        input(const input&) = delete;
        input& operator=(const input&) = delete;
        input(input&&) = default;
        input& operator=(input&&) = default;

        ~input() = default;

        /**
         * @brief Updates the tracked input states.
         */
        void update();

        [[nodiscard]] bool is_key_held(key key) const;
        [[nodiscard]] bool is_key_pressed(key key) const;
        [[nodiscard]] bool is_key_released(key key) const;

        [[nodiscard]] bool is_mouse_held(mouse_button btn) const;
        [[nodiscard]] bool is_mouse_pressed(mouse_button btn) const;
        [[nodiscard]] bool is_mouse_released(mouse_button btn) const;

        [[nodiscard]] math::vec2f mouse_position() const;
        [[nodiscard]] math::vec2f mouse_delta() const;
        [[nodiscard]] float mouse_wheel() const;

        void add_mouse_wheel(float w);

    private:
        std::array<bool, SDL_SCANCODE_COUNT> _prev_keys{};
        std::array<bool, SDL_SCANCODE_COUNT> _curr_keys{};

        std::array<bool, 3> _prev_mouse{};
        std::array<bool, 3> _curr_mouse{};

        math::vec2f _mouse_pos{};
        math::vec2f _prev_mouse_pos{};
        math::vec2f _mouse_delta{};

        float _mouse_wheel = 0.0f;
    };

} // namespace cas::inputs
