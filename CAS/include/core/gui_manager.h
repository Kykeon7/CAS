#pragma once
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

namespace cas {

class GuiManager {
public:
    void init(SDL_Window* window, SDL_Renderer* renderer);
    void shutdown();

    void begin_frame();
    void end_frame(SDL_Renderer* renderer);

    void process_event(const SDL_Event& e);
};

}
