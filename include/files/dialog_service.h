#pragma once

#include <SDL3/SDL.h>

#include <platform/window.h>

#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>

namespace cas::files {

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

struct file_request {
    file_operation operation;
    std::filesystem::path path;
};

class dialog_service {
public:
    explicit dialog_service(platform::window& window);

    dialog_service(const dialog_service&) = delete;
    dialog_service& operator=(const dialog_service&) = delete;

    void open_dialog(file_operation operation);
    void request(file_operation operation);

    [[nodiscard]]
    std::optional<file_request> consume();

private:
    struct shared_state {
        void submit(file_request request);
        std::optional<file_request> consume();

    private:
        std::mutex mutex;
        std::optional<file_request> request;
    };

    struct dialog_context {
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