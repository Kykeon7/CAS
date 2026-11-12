
#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_gpu.h>
#include <imgui.h>

#include <memory>

#include <platform/gpu_device.h>
#include <platform/renderer.h>
#include <platform/window.h>

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

/**
 * @brief Manages the Dear ImGui context, rendering backend and GUI lifecycle.
 *
 * Supports both SDL renderer and SDL GPU backends.
 */
class manager {
public:
    /**
     * @brief Initializes the GUI using the SDL renderer backend.
     *
     * @param window Application window.
     * @param renderer SDL renderer.
     */
    manager(
        const platform::window& window,
        const platform::renderer& renderer
    );

    /**
     * @brief Initializes the GUI using the SDL GPU backend.
     *
     * @param device GPU device associated with the application window.
     */
    explicit manager(const platform::gpu_device& device);

    manager(const manager&) = delete;
    manager& operator=(const manager&) = delete;

    manager(manager&&) = delete;
    manager& operator=(manager&&) = delete;

    ~manager();

    /**
     * @brief Forwards an SDL event to the ImGui SDL backend.
     *
     * @param event SDL event to process.
     */
    void process_event(const SDL_Event& event);

    /**
     * @brief Applies the configured GUI appearance.
     *
     * @param settings Interface settings to apply.
     */
    void apply_interface_settings(const interface_settings& settings);

    /**
     * @brief RAII handle for a CPU-rendered GUI frame.
     *
     * The frame is rendered when this object is destroyed. Moving transfers
     * responsibility for ending the frame to the destination object.
     */
    class frame {
    public:
        /**
         * @brief Begins a CPU GUI frame.
         *
         * @param gui GUI manager.
         * @param renderer SDL renderer used to render the frame.
         */
        frame(manager& gui, platform::renderer& renderer);

        frame(const frame&) = delete;
        frame& operator=(const frame&) = delete;

        frame(frame&& other) noexcept;
        frame& operator=(frame&&) = delete;

        ~frame();

    private:
        manager* _gui;
        platform::renderer* _renderer;
    };

    /**
     * @brief Begins a CPU GUI frame.
     *
     * The returned object ends and renders the frame when destroyed.
     *
     * @param renderer SDL renderer used to render the frame.
     * @return RAII handle for the frame.
     */
    [[nodiscard]]
    frame begin_frame(platform::renderer& renderer);

    /**
     * @brief Begins a GPU GUI frame.
     */
    void begin_frame();

    /**
     * @brief Discards the current GUI frame without rendering it.
     */
    void cancel_frame() noexcept;

    /**
     * @brief Finalizes the current GUI frame and prepares its draw data.
     *
     * @param command_buffer GPU command buffer used for preparation.
     */
    void prepare_draw_data(SDL_GPUCommandBuffer* command_buffer);

    /**
     * @brief Renders previously prepared GUI draw data.
     *
     * @param command_buffer GPU command buffer used for rendering.
     * @param render_pass Active GPU render pass.
     */
    void render_draw_data(
        SDL_GPUCommandBuffer* command_buffer,
        SDL_GPURenderPass* render_pass
    );

    /**
     * @brief Begins the root dockspace window.
     */
    void begin_dockspace();

    /**
     * @brief Submits the main dockspace.
     */
    void submit_dockspace();

    /**
     * @brief Ends the root dockspace window.
     */
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

} // namespace cas::gui
