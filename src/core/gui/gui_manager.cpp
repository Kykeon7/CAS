#include <core/gui/gui_manager.h>
#include <imgui_internal.h>

namespace cas {

    GuiManager::GuiManager(
        const platform::Window& window,
        const platform::Renderer& renderer
    )
    {
        IMGUI_CHECKVERSION();

        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLRenderer(
            window.native(),
            renderer.native()
        );

        ImGui_ImplSDLRenderer3_Init(
            renderer.native()
        );
    }

    GuiManager::~GuiManager()
    {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();

        ImGui::DestroyContext();
    }


    void GuiManager::process_event(const SDL_Event& e)
    {
        ImGui_ImplSDL3_ProcessEvent(&e);
    }


    GuiManager::Frame GuiManager::begin_frame(
        platform::Renderer& renderer
    )
    {
        return Frame(*this, renderer);
    }


    void GuiManager::begin_frame_internal()
    {
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();

        ImGui::NewFrame();
    }


    void GuiManager::end_frame_internal(
        platform::Renderer& renderer
    )
    {
        ImGui::Render();

        ImGui_ImplSDLRenderer3_RenderDrawData(
            ImGui::GetDrawData(),
            renderer.native()
        );
    }


    GuiManager::Frame::Frame(
        GuiManager& gui,
        platform::Renderer& renderer
    )
        : gui_(gui)
        , renderer_(renderer)
    {
        gui_.begin_frame_internal();
    }


    GuiManager::Frame::~Frame()
    {
        gui_.end_frame_internal(renderer_);
    }


    void GuiManager::begin_dockspace()
    {
        ImGuiViewport* viewport =
            ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus |
            ImGuiWindowFlags_NoBackground;

        ImGui::Begin(
            "DockSpaceRoot",
            nullptr,
            flags
        );

        ImGuiID dockspace_id =
            ImGui::GetID("MainDockSpace");

        static bool first_time = true;

        if (first_time) {
            first_time = false;

            ImGui::DockBuilderRemoveNode(
                dockspace_id
            );

            ImGui::DockBuilderAddNode(
                dockspace_id,
                ImGuiDockNodeFlags_DockSpace
            );

            ImGui::DockBuilderSetNodeSize(
                dockspace_id,
                viewport->Size
            );

            ImGuiID dock_main = dockspace_id;

            ImGuiID dock_left =
                ImGui::DockBuilderSplitNode(
                    dock_main,
                    ImGuiDir_Left,
                    0.20f,
                    nullptr,
                    &dock_main
                );

            ImGuiID dock_right =
                ImGui::DockBuilderSplitNode(
                    dock_main,
                    ImGuiDir_Right,
                    0.25f,
                    nullptr,
                    &dock_main
                );

            ImGuiID dock_bottom =
                ImGui::DockBuilderSplitNode(
                    dock_main,
                    ImGuiDir_Down,
                    0.25f,
                    nullptr,
                    &dock_main
                );

            ImGui::DockBuilderDockWindow(
                "Simulation",
                dock_left
            );

            ImGui::DockBuilderDockWindow(
                "Appearance",
                dock_left
            );

            ImGui::DockBuilderDockWindow(
                "Help",
                dock_right
            );

            ImGui::DockBuilderDockWindow(
                "Stats",
                dock_right
            );

            ImGui::DockBuilderDockWindow(
                "Graphs",
                dock_bottom
            );

            ImGui::DockBuilderDockWindow(
                "Viewport",
                dock_main
            );

            ImGui::DockBuilderFinish(
                dockspace_id
            );
        }

        ImGui::DockSpace(dockspace_id);
    }


    void GuiManager::end_dockspace()
    {
        ImGui::End();
    }

} // namespace cas