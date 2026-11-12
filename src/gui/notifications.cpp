#include <gui/notifications.h>

#include <imgui.h>

#include <system/logger.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <string>
#include <utility>

namespace cas::gui {
namespace {

constexpr float toast_width = 340.0f;
constexpr float toast_margin = 16.0f;

/**
 * @brief Returns the display name associated with a notification level.
 */
const char* level_name(notification_level level) {
    switch (level) {
    case notification_level::info:
        return "Information";
    case notification_level::warning:
        return "Warning";
    case notification_level::error:
        return "Error";
    }

    return "Notification";
}

constexpr const char* blocking_popup_name = "CAS Loading";
constexpr const char* error_popup_name = "CAS Error";

} // namespace

void notifications::info(
    std::string_view category,
    std::string_view message,
    std::chrono::milliseconds duration
) {
    enqueue(
        notification_level::info,
        category,
        message,
        duration
    );
}

void notifications::warning(
    std::string_view category,
    std::string_view message,
    std::chrono::milliseconds duration
) {
    enqueue(
        notification_level::warning,
        category,
        message,
        duration
    );
}

void notifications::error(
    std::string_view category,
    std::string_view message
) {
    auto& logger = system::logger::instance();

    logger.error(category, message);

    notification item{
        .id = 0,
        .level = notification_level::error,
        .category = std::string(category),
        .message = std::string(message),
        .blocking = true,
        .duration = std::chrono::milliseconds::zero()
    };

    std::lock_guard lock(_mutex);

    _pending.push_back({
        .type = command_type::add,
        .value = std::move(item),
        .id = 0
    });
}

notification_id notifications::begin_blocking(
    std::string_view category,
    std::string_view message
) {
    auto& logger = system::logger::instance();

    logger.info(category, message);

    notification item{
        .id = 0,
        .level = notification_level::info,
        .category = std::string(category),
        .message = std::string(message),
        .blocking = true,
        .duration = std::chrono::milliseconds::zero()
    };

    std::lock_guard lock(_mutex);

    item.id = _next_id++;

    const auto id = item.id;

    _pending.push_back({
        .type = command_type::add,
        .value = std::move(item),
        .id = id
    });

    return id;
}

void notifications::dismiss(notification_id id) {
    if (id == 0) {
        return;
    }

    std::lock_guard lock(_mutex);

    _pending.push_back({
        .type = command_type::dismiss,
        .value = {},
        .id = id
    });
}

void notifications::update_progress(
    notification_id id,
    std::uint64_t completed,
    std::uint64_t total,
    std::string_view stage
) {
    if (id == 0 || total == 0)
        return;

    notification value;

    value.progress = notification_progress{
        .completed = completed,
        .total = total,
        .stage = std::string(stage)
    };

    std::lock_guard lock(_mutex);

    _pending.push_back({
        .type = command_type::update_progress,
        .value = std::move(value),
        .id = id
    });
}

void notifications::enqueue(
    notification_level level,
    std::string_view category,
    std::string_view message,
    std::chrono::milliseconds duration
) {
    auto& logger = system::logger::instance();

    switch (level) {
    case notification_level::info:
        logger.info(category, message);
        break;
    case notification_level::warning:
        logger.warn(category, message);
        break;
    case notification_level::error:
        logger.error(category, message);
        break;
    }

    notification item{
        .id = 0,
        .level = level,
        .category = std::string(category),
        .message = std::string(message),
        .blocking = false,
        .duration = std::max(
            duration,
            std::chrono::milliseconds::zero()
        )
    };

    std::lock_guard lock(_mutex);

    _pending.push_back({
        .type = command_type::add,
        .value = std::move(item),
        .id = 0
    });
}

void notifications::render() {
    std::deque<command> pending;

    {
        std::lock_guard lock(_mutex);
        pending.swap(_pending);
    }

    for (auto& item : pending) {
        if (item.type == command_type::dismiss) {
            std::erase_if(
                _blocking,
                [&item](const blocking_notification& blocking) {
                    return blocking.value.id == item.id;
                }
            );

            continue;
        }

        if (item.type == command_type::update_progress) {
            const auto it = std::find_if(
                _blocking.begin(),
                _blocking.end(),
                [&item](const blocking_notification& blocking) {
                    return blocking.value.id == item.id;
                }
            );

            if (it != _blocking.end()) {
                it->value.progress = item.value.progress;
            }

            continue;
        }

        if (item.value.level == notification_level::error) {
            _errors.push_back(std::move(item.value));
        }
        else if (item.value.blocking) {
            _blocking.push_back({
                .value = std::move(item.value),
                .dismissed = false
                });
        }
        else {
            const auto now = std::chrono::steady_clock::now();

            const auto duration = item.value.duration;
            const auto expires_at = now + duration;

            _toasts.push_back({
                .value = std::move(item.value),
                .expires_at = expires_at
                });
        }
    }

    const auto now = std::chrono::steady_clock::now();

    std::erase_if(
        _toasts,
        [now](const toast& item) {
            return now >= item.expires_at;
        }
    );

    render_toasts();
    render_blocking();
    render_error_modal();
}

void notifications::render_toasts() {
    if (_toasts.empty()) {
        return;
    }

    const ImGuiIO& io = ImGui::GetIO();
    float y = toast_margin;

    for (std::size_t i = 0; i < _toasts.size(); ++i) {
        const auto& item = _toasts[i];

        ImGui::SetNextWindowPos(
            ImVec2(
                io.DisplaySize.x - toast_width - toast_margin,
                y
            ),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowSize(
            ImVec2(toast_width, 0.0f),
            ImGuiCond_Always
        );

        ImGui::SetNextWindowBgAlpha(0.95f);

        const std::string title =
            std::string(level_name(item.value.level)) +
            "##notification_" + std::to_string(i);

        constexpr ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav;

        float height = 0.0f;

        if (ImGui::Begin(title.c_str(), nullptr, flags)) {
            ImGui::TextUnformatted(item.value.category.c_str());
            ImGui::Separator();
            ImGui::TextWrapped("%s", item.value.message.c_str());

            height = ImGui::GetWindowHeight();
        }

        ImGui::End();

        y += height + 8.0f;
    }
}


void notifications::render_blocking() {
    if (_blocking.empty()) {
        if (ImGui::BeginPopupModal(blocking_popup_name)) {
            ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        return;
    }

    const auto& item = _blocking.front().value;

    if (!ImGui::IsPopupOpen(blocking_popup_name)) {
        ImGui::OpenPopup(blocking_popup_name);
    }

    const ImGuiIO& io = ImGui::GetIO();

    ImGui::SetNextWindowPos(
        ImVec2(
            io.DisplaySize.x * 0.5f,
            io.DisplaySize.y * 0.5f
        ),
        ImGuiCond_Appearing,
        ImVec2(0.5f, 0.5f)
    );

    // Tamaño estable, no depende de las dimensiones del contenido.
    ImGui::SetNextWindowSize(
        ImVec2(420.0f, 170.0f),
        ImGuiCond_Appearing
    );

    if (ImGui::BeginPopupModal(
        blocking_popup_name,
        nullptr,
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize
    )) {
        ImGui::TextUnformatted(item.category.c_str());
        ImGui::Separator();

        ImGui::TextWrapped("%s", item.message.c_str());

        ImGui::Spacing();

        if (item.progress.has_value()
            && item.progress->total > 0) {
            const auto& progress = *item.progress;

            const double fraction = std::clamp(
                static_cast<double>(progress.completed) /
                static_cast<double>(progress.total),
                0.0,
                1.0
            );

            const int percentage = static_cast<int>(
                fraction * 100.0
                );

            if (!progress.stage.empty()) {
                ImGui::TextWrapped(
                    "%s",
                    progress.stage.c_str()
                );

                ImGui::Spacing();
            }

            const std::string overlay =
                std::to_string(percentage) + "%";

            ImGui::ProgressBar(
                static_cast<float>(fraction),
                ImVec2(-1.0f, 0.0f),
                overlay.c_str()
            );
        }
        else {
            ImGui::TextUnformatted("Loading...");

            const float animation_speed = 0.8f;

            const float progress = std::fmod(
                static_cast<float>(ImGui::GetTime()) * animation_speed,
                1.0f
            );

            ImGui::ProgressBar(
                progress,
                ImVec2(-1.0f, 0.0f),
                ""
            );
        }

        ImGui::EndPopup();
    }
}

void notifications::render_error_modal() {
    if (_active_error.has_value()) {
        // Mantener visible el error activo hasta que el usuario
        // lo cierre. Los errores posteriores esperan en la cola.
    }
    else if (!_errors.empty() && _blocking.empty()) {
        _active_error = std::move(_errors.front());
        _errors.pop_front();
    }

    if (!_active_error.has_value()) {
        return;
    }

    if (!ImGui::IsPopupOpen(error_popup_name)) {
        ImGui::OpenPopup(error_popup_name);
    }

    ImGui::SetNextWindowSize(
        ImVec2(440.0f, 0.0f),
        ImGuiCond_Appearing
    );

    if (ImGui::BeginPopupModal(
        error_popup_name,
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings
    )) {
        ImGui::TextUnformatted(
            _active_error->category.c_str()
        );

        ImGui::Separator();

        ImGui::TextWrapped(
            "%s",
            _active_error->message.c_str()
        );

        ImGui::Spacing();

        if (ImGui::Button("OK", ImVec2(100.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            _active_error.reset();
        }

        ImGui::EndPopup();
    }
}

} // namespace cas
