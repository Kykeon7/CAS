#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <imgui.h>

#include <platform/renderer.h>
#include <platform/gpu_device.h>
#include <platform/window.h>

#include <memory>

namespace cas {
    struct interface_settings;
}

namespace cas::gui {

enum class left_panel {
    simulation,
    appearance,
    file
};

enum class right_panel {
    stats,
    help
};

class gui_backend;

class manager {
public:
    // CPU backend.
    manager(
        const platform::window& window,
        const platform::renderer& renderer
    );

    // GPU backend.
    explicit manager(
        const platform::gpu_device& device
    );

    ~manager();

    manager(const manager&) = delete;
    manager& operator=(const manager&) = delete;

    void process_event(const SDL_Event& event);
    void apply_interface_settings(
        const interface_settings& settings
    );

    // CPU frame: RAII; renders when the frame object is destroyed.
    class frame {
    public:
        frame(
            manager& gui,
            platform::renderer& renderer
        );

        ~frame();

        frame(const frame&) = delete;
        frame& operator=(const frame&) = delete;

        frame(frame&& other) noexcept;
        frame& operator=(frame&&) = delete;

    private:
        manager* _gui;
        platform::renderer* _renderer;
    };

    [[nodiscard]]
    frame begin_frame(platform::renderer& renderer);

    // GPU frame: explicit lifecycle.
    void begin_frame();

    void prepare_draw_data(
        SDL_GPUCommandBuffer* command_buffer
    );

    void render_draw_data(
        SDL_GPUCommandBuffer* command_buffer,
        SDL_GPURenderPass* render_pass
    );

    void begin_dockspace();
    void submit_dockspace();
    void end_dockspace();

private:
    void begin_frame_internal();
    void end_frame_internal(platform::renderer& renderer);

    std::unique_ptr<gui_backend> _backend;

    bool _frame_started = false;
    bool _draw_data_prepared = false;
    bool _dockspace_initialized = false;

    left_panel _active_left_panel = left_panel::simulation;
    right_panel _active_right_panel = right_panel::stats;
};

} // namespace cas