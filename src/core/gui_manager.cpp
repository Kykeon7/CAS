#include "core/gui_manager.h"

namespace cas {

GuiManager::GuiManager(const platform::Window& window,
    const platform::Renderer& renderer) 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window.native(), renderer.native());
    ImGui_ImplSDLRenderer3_Init(renderer.native());
}

GuiManager::~GuiManager() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::process_event(const SDL_Event& e) {
    ImGui_ImplSDL3_ProcessEvent(&e);
}

GuiManager::Frame GuiManager::begin_frame(platform::Renderer& renderer) {
    return Frame(*this, renderer);
}

GuiManager::Frame::Frame(GuiManager& gui,
    platform::Renderer& renderer)
    : gui_(gui), renderer_(renderer)
{
    gui_.begin_frame_internal();
}

GuiManager::Frame::~Frame() {
    gui_.end_frame_internal(renderer_);
}

void GuiManager::begin_frame_internal() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void GuiManager::end_frame_internal(platform::Renderer& renderer) {
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(
        ImGui::GetDrawData(),
        renderer.native()
    );
}

}
