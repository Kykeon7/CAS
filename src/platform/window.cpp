#include "platform/window.h"

#include <SDL3/SDL.h>

#include <filesystem>
#include <stdexcept>
#include <string>


namespace cas::platform {

window::window(std::string_view title) {
    const std::string title_string(title);

    _handle = SDL_CreateWindow(
        title_string.c_str(),
        1280,
        800,
        SDL_WINDOW_HIGH_PIXEL_DENSITY |
        SDL_WINDOW_RESIZABLE |
        SDL_WINDOW_MAXIMIZED
    );

    if (!_handle)
        throw std::runtime_error(SDL_GetError());

    set_icon();
}

window::window(
    std::string_view title,
    int width,
    int height
) {
    const std::string title_string(title);

    _handle = SDL_CreateWindow(
        title_string.c_str(),
        width,
        height,
        SDL_WINDOW_HIGH_PIXEL_DENSITY |
        SDL_WINDOW_RESIZABLE
    );

    if (!_handle)
        throw std::runtime_error(SDL_GetError());

    set_icon();
}

window::~window() {
    if (_handle)
        SDL_DestroyWindow(_handle);
}

void window::set_title(std::string_view title) {
    const std::string title_string(title);

    if (!SDL_SetWindowTitle(_handle, title_string.c_str()))
        throw std::runtime_error(SDL_GetError());
}

void window::set_full_screen(bool enabled) {
    if (!SDL_SetWindowFullscreen(_handle, enabled))
        throw std::runtime_error(SDL_GetError());

    _is_full_screen = enabled;
}

void window::set_icon() {
    const std::filesystem::path icon_path =
        std::filesystem::path(SDL_GetBasePath()) /
        "resources" / "icons" / "icon.bmp";

    SDL_Surface* icon = SDL_LoadBMP(icon_path.string().c_str());

    if (!icon)
        throw std::runtime_error(SDL_GetError());

    const bool success = SDL_SetWindowIcon(_handle, icon);

    SDL_DestroySurface(icon);

    if (!success)
        throw std::runtime_error(SDL_GetError());
}

}