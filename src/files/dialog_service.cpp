#include <files/dialog_service.h>

#include <array>
#include <memory>
#include <stdexcept>
#include <utility>

namespace cas::files {

namespace {

constexpr std::array<SDL_DialogFileFilter, 1> json_filters{{
    { "JSON files", "json" }
}};

bool is_save_operation(file_operation operation) noexcept {
    switch (operation) {
    case file_operation::save_automaton:
    case file_operation::save_viewport:
    case file_operation::save_workspace_as:
        return true;

    default:
        return false;
    }
}

} // namespace

void dialog_service::shared_state::submit(
    file_request new_request
) {
    std::lock_guard lock(mutex);
    request = std::move(new_request);
}

std::optional<file_request>
dialog_service::shared_state::consume() {
    std::lock_guard lock(mutex);

    auto result = std::move(request);
    request.reset();

    return result;
}

dialog_service::dialog_service(platform::window& window)
    : _window(window),
      _state(std::make_shared<shared_state>())
{}

void dialog_service::open_dialog(file_operation operation) {
    auto context = std::make_unique<dialog_context>(
        dialog_context{ _state, operation }
    );

    auto* context_ptr = context.get();

    if (operation == file_operation::select_save_directory) {
        SDL_ShowOpenFolderDialog(
            dialog_callback,
            context_ptr,
            _window.native(),
            nullptr,
            false
        );
    }
    else if (is_save_operation(operation)) {
        SDL_ShowSaveFileDialog(
            dialog_callback,
            context_ptr,
            _window.native(),
            json_filters.data(),
            static_cast<int>(json_filters.size()),
            nullptr
        );
    }
    else {
        SDL_ShowOpenFileDialog(
            dialog_callback,
            context_ptr,
            _window.native(),
            json_filters.data(),
            static_cast<int>(json_filters.size()),
            nullptr,
            false
        );
    }

    context.release();
}

void dialog_service::request(file_operation operation) {
    _state->submit({
        operation,
        {}
    });
}

std::optional<file_request> dialog_service::consume() {
    return _state->consume();
}

void SDLCALL dialog_service::dialog_callback(
    void* userdata,
    const char* const* filelist,
    int
) {
    std::unique_ptr<dialog_context> context(
        static_cast<dialog_context*>(userdata)
    );

    if (!filelist || !filelist[0])
        return;

    try {
        auto path = std::filesystem::u8path(filelist[0]);

        if (is_save_operation(context->operation) &&
            path.extension().empty()) {
            path += ".json";
        }

        context->state->submit({
            context->operation,
            std::move(path)
        });
    }
    catch (...) {
        // Exceptions must not escape an SDL callback.
    }
}

} // namespace cas::files