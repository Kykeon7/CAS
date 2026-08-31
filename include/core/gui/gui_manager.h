#pragma once

#include <SDL3/SDL.h>
#include <imgui.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <platform/window.h>
#include <core/rendering/renderer.h>

namespace cas {

class GuiManager {
public:
    explicit GuiManager(const platform::Window& window,
        const platform::Renderer& renderer);
    ~GuiManager();

    GuiManager(const GuiManager&) = delete;
    GuiManager& operator=(const GuiManager&) = delete;
    void begin_dockspace();
    void end_dockspace();
    void process_event(const SDL_Event& e);

    class Frame {
    public:
        Frame(GuiManager& gui, platform::Renderer& renderer);
        ~Frame();

        Frame(const Frame&) = delete;
        Frame& operator=(const Frame&) = delete;

    private:
        GuiManager& gui_;
        platform::Renderer& renderer_;
    };

    [[nodiscard]] Frame begin_frame(platform::Renderer& renderer);

private:
    void begin_frame_internal();
    void end_frame_internal(platform::Renderer& renderer);
};

}