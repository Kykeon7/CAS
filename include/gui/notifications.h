#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cas::gui {

using notification_id = std::uint64_t;

enum class notification_level {
    info,
    warning,
    error
};

struct notification_progress {
    std::uint64_t completed = 0;
    std::uint64_t total = 0;
    std::string stage;
};

struct notification {
    notification_id id = 0;
    notification_level level = notification_level::info;
    std::string category;
    std::string message;
    bool blocking = false;
    std::chrono::milliseconds duration{5000};
    std::optional<notification_progress> progress;
};

class notifications final {
public:
    notifications() = default;

    notifications(const notifications&) = delete;
    notifications& operator=(const notifications&) = delete;

    void info(
        std::string_view category,
        std::string_view message,
        std::chrono::milliseconds duration =
            std::chrono::seconds(5)
    );

    void warning(
        std::string_view category,
        std::string_view message,
        std::chrono::milliseconds duration =
            std::chrono::seconds(5)
    );

    void error(
        std::string_view category,
        std::string_view message
    );

    [[nodiscard]] notification_id begin_blocking(
        std::string_view category,
        std::string_view message
    );

    void dismiss(notification_id id);

    void update_progress(
        notification_id id,
        std::uint64_t completed,
        std::uint64_t total,
        std::string_view stage = {}
    );

    // Solo desde el hilo principal durante un frame de ImGui.
    void render();

private:
    struct toast {
        notification value;
        std::chrono::steady_clock::time_point expires_at;
    };

    struct blocking_notification {
        notification value;
        bool dismissed = false;
    };

    enum class command_type {
        add,
        dismiss,
        update_progress
    };

    struct command {
        command_type type = command_type::add;
        notification value;
        notification_id id = 0;
    };

    void enqueue(
        notification_level level,
        std::string_view category,
        std::string_view message,
        std::chrono::milliseconds duration
    );

    void render_toasts();
    void render_blocking();
    void render_error_modal();

    std::mutex _mutex;
    std::deque<command> _pending;
    std::vector<toast> _toasts;
    std::deque<blocking_notification> _blocking;
    std::deque<notification> _errors;

    std::optional<notification> _active_error;

    notification_id _next_id = 1;
    bool _error_popup_open_requested = false;
};

} // namespace cas