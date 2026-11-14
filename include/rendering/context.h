#pragma once

#include <memory>

#include <rendering/backend.h>
#include <rendering/resources.h>

namespace cas {
namespace viewports {
    class camera;
    class viewport;
}

namespace platform {
    class window;
    class renderer;
    class gpu_device;
    class gpu_renderer;
}
}

namespace cas::rendering {

namespace automaton {
    class renderer;
}

/**
 * @brief Manages the rendering resources and backend used by the application.
 *
 * Provides access to the platform rendering resources and creates
 * automaton renderers according to the selected backend.
 *
 * The context does not own the window passed to its constructor.
 * The window must outlive the context.
 */
class context {
public:
    /**
     * Creates a rendering context using the specified backend.
     *
     * @param window Window used to initialize the rendering resources.
     * @param backend Rendering backend to use.
     */
    context(
        cas::platform::window& window,
        rendering::backend backend
    );

    /** Releases the rendering resources owned by the context. */
    ~context();

    context(const context&) = delete;
    context& operator=(const context&) = delete;
    context(context&&) = delete;
    context& operator=(context&&) = delete;

    /**
     * Returns the selected rendering backend.
     *
     * @return Active rendering backend.
     */
    [[nodiscard]]
    rendering::backend backend() const noexcept;

    /**
     * Checks whether the CPU backend is selected.
     *
     * @return True if the CPU backend is selected.
     */
    [[nodiscard]]
    bool is_cpu_backend() const noexcept;

    /**
     * Checks whether the GPU backend is selected.
     *
     * @return True if the GPU backend is selected.
     */
    [[nodiscard]]
    bool is_gpu_backend() const noexcept;

    /**
     * Initializes the resources required by the selected backend.
     *
     * For the CPU backend, creates the platform renderer using the
     * supplied camera. For the GPU backend, creates the GPU device
     * and its renderer.
     *
     * @param initial_camera Camera used to initialize the CPU renderer.
     * @throws std::logic_error If the context has already been initialized.
     */
    void initialize(cas::viewports::camera& initial_camera);

    /**
     * Selects a backend before the context is initialized.
     *
     * @param backend Backend to select.
     * @throws std::logic_error If the context has already been initialized.
     */
    void set_backend(rendering::backend backend);

    /**
     * Replaces the backend resources and returns the context to an uninitialized state.
     *
     * All renderers and other resources depending on this context must be
     * destroyed before calling this function.
     *
     * @param backend Backend to select.
     */
    void reset_backend(rendering::backend backend);

    /**
     * Checks whether the context has been initialized.
     *
     * @return True if initialization has completed successfully.
     */
    [[nodiscard]]
    bool initialized() const noexcept;

    /**
     * Returns the CPU renderer, if available.
     *
     * @return Non-owning pointer to the CPU renderer, or nullptr if
     *         the CPU backend is not selected or the renderer is unavailable.
     */
    [[nodiscard]]
    cas::platform::renderer* cpu_renderer() noexcept;

    /**
     * Returns the GPU device, if available.
     *
     * @return Non-owning pointer to the GPU device, or nullptr if
     *         the GPU backend is not selected or the device is unavailable.
     */
    [[nodiscard]]
    cas::platform::gpu_device* gpu_device() noexcept;

    /**
     * Returns the GPU renderer, if available.
     *
     * @return Non-owning pointer to the GPU renderer, or nullptr if
     *         the GPU backend is not selected or the renderer is unavailable.
     */
    [[nodiscard]]
    cas::platform::gpu_renderer* gpu_renderer() noexcept;

    /**
     * Creates an automaton renderer compatible with the selected backend.
     *
     * @param viewport Viewport whose layout is used by the CPU renderer.
     * @return Owning pointer to the created automaton renderer.
     * @throws std::logic_error If the context has not been initialized.
     */
    [[nodiscard]]
    std::unique_ptr<cas::rendering::automaton::renderer>
    create_automaton_renderer(cas::viewports::viewport& viewport);

private:
    cas::platform::window& _window;
    rendering_resources _resources;
    rendering::backend _backend;
    bool _initialized = false;
};

} // namespace cas::rendering