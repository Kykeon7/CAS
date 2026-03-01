#include "core/gui_manager.h"

namespace cas {

void GuiManager::init(SDL_Window* window, SDL_Renderer* renderer) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);
}

void GuiManager::shutdown() {
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void GuiManager::begin_frame() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void GuiManager::end_frame(SDL_Renderer* renderer) {
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void GuiManager::process_event(const SDL_Event& e) {
    ImGui_ImplSDL3_ProcessEvent(&e);
}

}
