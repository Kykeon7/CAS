
#include "gui/manager.h"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <imgui_impl_sdlgpu3.h>
#include <implot.h>
#include <stdexcept>
#include <utility>
#include <imgui_internal.h>
#include <gui/style.h>

namespace cas::gui {

class gui_backend {
public:
    virtual ~gui_backend() = default;

    [[nodiscard]]
    virtual bool is_gpu() const noexcept = 0;

    virtual void new_frame() = 0;

    virtual void render_cpu_draw_data(
        platform::renderer& renderer
    ) = 0;

    virtual void prepare_draw_data(
        SDL_GPUCommandBuffer* command_buffer
    ) = 0;

    virtual void render_draw_data(
        SDL_GPUCommandBuffer* command_buffer,
        SDL_GPURenderPass* render_pass
    ) = 0;
};

namespace {

void initialize_imgui_context() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui::StyleColorsDark();

    io.FontGlobalScale = 1.25f;

    io.IniFilename = nullptr;
}

class cpu_gui_backend final : public gui_backend {
public:
    cpu_gui_backend(
        const platform::window& window,
        const platform::renderer& renderer
    ) {
        if (!ImGui_ImplSDL3_InitForSDLRenderer(
                window.native(),
                renderer.native()
            )) {
            throw std::runtime_error(
                "Failed to initialize ImGui SDL3 backend."
            );
        }

        if (!ImGui_ImplSDLRenderer3_Init(renderer.native())) {
            ImGui_ImplSDL3_Shutdown();

            throw std::runtime_error(
                "Failed to initialize ImGui SDL Renderer backend."
            );
        }
    }

    ~cpu_gui_backend() override {
        ImGui_ImplSDLRenderer3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    }

    [[nodiscard]]
    bool is_gpu() const noexcept override {
        return false;
    }

    void new_frame() override {
        ImGui_ImplSDLRenderer3_NewFrame();
    }

    void render_cpu_draw_data(
        platform::renderer& renderer
    ) override {
        ImGui_ImplSDLRenderer3_RenderDrawData(
            ImGui::GetDrawData(),
            renderer.native()
        );
    }

    void prepare_draw_data(
        SDL_GPUCommandBuffer*
    ) override {
        throw std::logic_error(
            "GPU draw-data preparation requested on the CPU backend."
        );
    }

    void render_draw_data(
        SDL_GPUCommandBuffer*,
        SDL_GPURenderPass*
    ) override {
        throw std::logic_error(
            "GPU draw-data rendering requested on the CPU backend."
        );
    }
};

class gpu_gui_backend final : public gui_backend {
public:
    explicit gpu_gui_backend(
        const platform::gpu_device& device
    ) {
        if (!ImGui_ImplSDL3_InitForOther(
                device.native_window().native()
            )) {
            throw std::runtime_error(
                "Failed to initialize ImGui SDL3 backend."
            );
        }

        ImGui_ImplSDLGPU3_InitInfo init_info{};
        init_info.Device = device.native();
        init_info.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(
            device.native(),
            device.native_window().native()
        );
        init_info.MSAASamples = SDL_GPU_SAMPLECOUNT_1;

        if (!ImGui_ImplSDLGPU3_Init(&init_info)) {
            ImGui_ImplSDL3_Shutdown();

            throw std::runtime_error(
                "Failed to initialize ImGui SDL GPU backend."
            );
        }
    }

    ~gpu_gui_backend() override {
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
    }

    [[nodiscard]]
    bool is_gpu() const noexcept override {
        return true;
    }

    void new_frame() override {
        ImGui_ImplSDLGPU3_NewFrame();
    }

    void render_cpu_draw_data(
        platform::renderer&
    ) override {
        throw std::logic_error(
            "CPU draw-data rendering requested on the GPU backend."
        );
    }

    void prepare_draw_data(
        SDL_GPUCommandBuffer* command_buffer
    ) override {
        ImGui_ImplSDLGPU3_PrepareDrawData(
            ImGui::GetDrawData(),
            command_buffer
        );
    }

    void render_draw_data(
        SDL_GPUCommandBuffer* command_buffer,
        SDL_GPURenderPass* render_pass
    ) override {
        ImGui_ImplSDLGPU3_RenderDrawData(
            ImGui::GetDrawData(),
            command_buffer,
            render_pass
        );
    }
};

} // namespace

manager::manager(
    const platform::window& window,
    const platform::renderer& renderer
) {
    initialize_imgui_context();

    try {
        _backend = std::make_unique<cpu_gui_backend>(
            window,
            renderer
        );
    } catch (...) {
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        throw;
    }
}

manager::manager(
    const platform::gpu_device& device
) {
    initialize_imgui_context();

    try {
        _backend = std::make_unique<gpu_gui_backend>(device);
    } catch (...) {
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
        throw;
    }
}

manager::~manager() {
    // Shut down the SDL integration before destroying the context.
    _backend.reset();

    if (ImGui::GetCurrentContext() != nullptr) {
        ImPlot::DestroyContext();
        ImGui::DestroyContext();
    }
}

void manager::process_event(const SDL_Event& event) {
    ImGui_ImplSDL3_ProcessEvent(&event);
}

void manager::apply_interface_settings(
    const interface_settings& settings
) {
    apply_interface_style(settings);
}

// -----------------------------------------------------------------------------
// CPU frame
// -----------------------------------------------------------------------------

manager::frame::frame(
    manager& gui,
    platform::renderer& renderer
)
    : _gui(&gui),
      _renderer(&renderer) {
    if (gui._backend->is_gpu()) {
        throw std::logic_error(
            "CPU frame requested on the GPU backend."
        );
    }

    gui.begin_frame_internal();
}

manager::frame::~frame() {
    if (_gui != nullptr) {
        _gui->end_frame_internal(*_renderer);
    }
}

manager::frame::frame(frame&& other) noexcept
    : _gui(std::exchange(other._gui, nullptr)),
      _renderer(std::exchange(other._renderer, nullptr)) {}

manager::frame manager::begin_frame(
    platform::renderer& renderer
) {
    if (_backend->is_gpu()) {
        throw std::logic_error(
            "CPU frame requested on the GPU backend."
        );
    }

    return frame(*this, renderer);
}

void manager::begin_frame_internal() {
    if (_frame_started) {
        throw std::logic_error(
            "An ImGui frame is already active."
        );
    }

    _backend->new_frame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    _frame_started = true;
}

void manager::end_frame_internal(
    platform::renderer& renderer
) {
    if (!_frame_started) {
        return;
    }

    ImGui::Render();

    _backend->render_cpu_draw_data(renderer);

    _frame_started = false;
}

// -----------------------------------------------------------------------------
// GPU frame
// -----------------------------------------------------------------------------

void manager::begin_frame() {
    if (!_backend->is_gpu()) {
        throw std::logic_error(
            "GPU frame requested on the CPU backend."
        );
    }

    // A previous frame may have produced draw data that could not be
    // rendered because the swapchain had no available texture.
    _draw_data_prepared = false;

    begin_frame_internal();
}

void manager::prepare_draw_data(
    SDL_GPUCommandBuffer* command_buffer
) {
    if (!_backend->is_gpu()) {
        throw std::logic_error(
            "GPU draw-data preparation requested on the CPU backend."
        );
    }

    if (!_frame_started) {
        throw std::logic_error(
            "Cannot prepare draw data without an active frame."
        );
    }

    if (command_buffer == nullptr) {
        throw std::invalid_argument(
            "Command buffer must not be null."
        );
    }

    ImGui::Render();
    _frame_started = false;

    _backend->prepare_draw_data(command_buffer);
    _draw_data_prepared = true;
}

void manager::render_draw_data(
    SDL_GPUCommandBuffer* command_buffer,
    SDL_GPURenderPass* render_pass
) {
    if (!_backend->is_gpu()) {
        throw std::logic_error(
            "GPU draw-data rendering requested on the CPU backend."
        );
    }

    if (!_draw_data_prepared) {
        throw std::logic_error(
            "Draw data must be prepared before rendering."
        );
    }

    if (command_buffer == nullptr || render_pass == nullptr) {
        throw std::invalid_argument(
            "Command buffer and render pass must not be null."
        );
    }

    _backend->render_draw_data(command_buffer, render_pass);
    _draw_data_prepared = false;
}

// -----------------------------------------------------------------------------
// Dockspace
// -----------------------------------------------------------------------------

void manager::begin_dockspace() {
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    constexpr ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpaceRoot", nullptr, flags);
}

void manager::submit_dockspace() {
    const ImGuiID dockspace_id =
        ImGui::GetID("MainDockSpace");

    ImGui::DockSpace(dockspace_id);
}

void manager::end_dockspace() {
    ImGui::End();
}

} // namespace cas
