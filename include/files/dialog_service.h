#pragma once

#include <SDL3/SDL.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>

#include <platform/window.h>

namespace cas::files {

/// @brief Identifies the type of file operation requested by the application.
enum class file_operation {
    save_automaton,
    load_automaton,
    save_viewport,
    load_viewport,
    save_workspace,
    save_workspace_as,
    load_workspace,
    select_save_directory
};

/// @brief Represents a file operation request and its associated path.
struct file_request {
    file_operation operation;
    std::filesystem::path path;
};

/// @brief Manages asynchronous file dialogs and pending file requests.
///
/// Dialog operations are asynchronous. Each dialog retains shared state
/// until its callback completes, allowing pending dialogs to finish even
/// if the dialog service is destroyed.
class dialog_service {
public:
    /// @brief Creates a dialog service associated with a window.
    /// @param window Window that owns the file dialogs.
    explicit dialog_service(platform::window& window);

    dialog_service(const dialog_service&) = delete;
    dialog_service& operator=(const dialog_service&) = delete;

    dialog_service(dialog_service&&) = default;
    dialog_service& operator=(dialog_service&&) = delete;

    ~dialog_service() = default;

    /// @brief Opens the dialog associated with a file operation.
    /// @param operation Operation to perform after the user selects a path.
    ///
    /// The request is submitted asynchronously when the dialog callback
    /// completes. Destroying the dialog service does not invalidate the
    /// callback's shared state.
    void open_dialog(file_operation operation);

    /// @brief Submits a file operation request without opening a dialog.
    /// @param operation Operation to request.
    void request(file_operation operation);

    /// @brief Retrieves and removes the pending file request, if any.
    /// @return Pending request, or std::nullopt if none is available.
    [[nodiscard]]
    std::optional<file_request> consume();

private:
    struct shared_state {
        shared_state() = default;

        shared_state(const shared_state&) = delete;
        shared_state& operator=(const shared_state&) = delete;

        shared_state(shared_state&&) = delete;
        shared_state& operator=(shared_state&&) = delete;

        ~shared_state() = default;

        void submit(file_request request);
        std::optional<file_request> consume();

    private:
        std::mutex mutex;
        std::optional<file_request> request;
    };

    struct dialog_context {
        dialog_context(
            std::shared_ptr<shared_state> state,
            file_operation operation
        );

        dialog_context(const dialog_context&) = default;
        dialog_context& operator=(const dialog_context&) = default;

        dialog_context(dialog_context&&) noexcept = default;
        dialog_context& operator=(dialog_context&&) noexcept = default;

        ~dialog_context() = default;

        std::shared_ptr<shared_state> state;
        file_operation operation;
    };

    static void SDLCALL dialog_callback(
        void* userdata,
        const char* const* filelist,
        int filter
    );

    platform::window& _window;
    std::shared_ptr<shared_state> _state;
};

} // namespace cas::files