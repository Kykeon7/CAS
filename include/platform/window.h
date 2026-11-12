#pragma once

#include <SDL3/SDL.h>

#include <string_view>

namespace cas::platform {

/**
 * Manages an SDL window.
 *
 * Owns the underlying SDL_Window resource and provides operations for
 * changing its title and fullscreen state.
 */
class window {
public:
    /**
     * Creates a maximized, resizable window with default dimensions.
     *
     * The window is created with an initial size of 1280 × 800 pixels.
     *
     * @param title Initial window title.
     * @throws std::runtime_error If SDL fails to create the window.
     */
    explicit window(std::string_view title);

    /**
     * Creates a resizable window with the specified dimensions.
     *
     * @param title Initial window title.
     * @param width Initial window width in pixels.
     * @param height Initial window height in pixels.
     * @throws std::runtime_error If SDL fails to create the window.
     */
    window(std::string_view title, int width, int height);

    /** Destroys the underlying SDL window. */
    ~window();

    window(const window&) = delete;
    window& operator=(const window&) = delete;

    window(window&&) = delete;
    window& operator=(window&&) = delete;

    /**
     * Changes the window's fullscreen state.
     *
     * @param enabled Whether fullscreen mode should be enabled.
     * @throws std::runtime_error If SDL fails to change the fullscreen state.
     */
    void set_full_screen(bool enabled);

    /**
     * Changes the window title.
     *
     * @param title New window title.
     * @throws std::runtime_error If SDL fails to change the title.
     */
    void set_title(std::string_view title);

    /**
     * Returns the underlying SDL window.
     *
     * @return Non-owning pointer to the SDL window.
     */
    SDL_Window* native() const { return _handle; }

    /**
     * Returns whether the window is currently marked as fullscreen.
     *
     * @return True if fullscreen mode is enabled, false otherwise.
     */
    bool is_full_screen() const { return _is_full_screen; }

private:
    SDL_Window* _handle{};
    bool _is_full_screen = false;
};

} // namespace cas::platform