#pragma once

#include <string>
#include <string_view>
#include <SDL3/SDL.h>

namespace cas::platform {

class window {
public:
    explicit window(std::string_view title);
    window(std::string_view title, int width, int height);

    ~window();

    window(const window&) = delete;
    window& operator=(const window&) = delete;

    void set_full_screen(bool enabled);
    void set_title(std::string_view title);

    SDL_Window* native() const { return _handle; }
    bool is_full_screen() const { return _is_full_screen; }

private:
    SDL_Window* _handle{};
    bool _is_full_screen = false;
};

}