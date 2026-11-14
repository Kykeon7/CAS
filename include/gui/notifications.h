#pragma once

#include <chrono>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace cas::gui {

/**
 * @brief Unique identifier assigned to a blocking notification.
 */
using notification_id = std::uint64_t;

/**
 * @brief Severity level of a notification.
 */
enum class notification_level {
    info,
    warning,
    error
};

/**
 * @brief Progress information associated with a notification.
 */
struct notification_progress {
    std::uint64_t completed = 0;
    std::uint64_t total = 0;
    std::string stage;
};

/**
 * @brief Data associated with a notification.
 */
struct notification {
    notification_id id = 0;
    notification_level level = notification_level::info;
    std::string category;
    std::string message;
    bool blocking = false;
    std::chrono::milliseconds duration{5000};
    std::optional<notification_progress> progress;
};

/**
 * @brief Manages application notifications and their GUI presentation.
 *
 * Notification requests can be submitted from other threads. They are
 * transferred to the main thread through an internal command queue, where
 * they are processed and rendered using Dear ImGui.
 */
class notifications final {
public:
    notifications() = default;

    notifications(const notifications&) = delete;
    notifications& operator=(const notifications&) = delete;

    notifications(notifications&&) = delete;
    notifications& operator=(notifications&&) = delete;

    ~notifications() = default;

    /**
     * @brief Enqueues an informational toast notification.
     *
     * @param category Notification category.
     * @param message Notification message.
     * @param duration Time before the notification expires.
     */
    void info(
        std::string_view category,
        std::string_view message,
        std::chrono::milliseconds duration =
            std::chrono::seconds(5)
    );

    /**
     * @brief Enqueues a warning toast notification.
     *
     * @param category Notification category.
     * @param message Notification message.
     * @param duration Time before the notification expires.
     */
    void warning(
        std::string_view category,
        std::string_view message,
        std::chrono::milliseconds duration =
            std::chrono::seconds(5)
    );

    /**
     * @brief Enqueues an error notification.
     *
     * The error is also written to the application logger.
     *
     * @param category Notification category.
     * @param message Error message.
     */
    void error(
        std::string_view category,
        std::string_view message
    );

    /**
     * @brief Enqueues a confirmation dialog.
     *
     * @param category Dialog title.
     * @param message Confirmation message.
     * @param on_result Callback invoked with true when confirmed
     *                  or false when cancelled.
     */
    void confirm(
        std::string_view category,
        std::string_view message,
        std::function<void(bool)> on_result,
        std::string_view confirm_label = "Confirm"
    );

    /**
     * @brief Starts a blocking notification.
     *
     * @param category Notification category.
     * @param message Notification message.
     * @return Identifier used to update or dismiss the notification.
     */
    [[nodiscard]]
    notification_id begin_blocking(
        std::string_view category,
        std::string_view message
    );

    /**
     * @brief Requests dismissal of a blocking notification.
     *
     * @param id Identifier of the notification to dismiss.
     */
    void dismiss(notification_id id);

    /**
     * @brief Updates the progress of a blocking notification.
     *
     * @param id Notification identifier.
     * @param completed Completed units.
     * @param total Total number of units.
     * @param stage Optional description of the current stage.
     */
    void update_progress(
        notification_id id,
        std::uint64_t completed,
        std::uint64_t total,
        std::string_view stage = {}
    );

    /**
     * @brief Processes pending commands and renders notifications.
     *
     * Must be called from the main thread during an active ImGui frame.
     */
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
        update_progress,
        confirm
    };

    struct command {
        command_type type = command_type::add;
        notification value;
        notification_id id = 0;
        std::function<void(bool)> on_result;
        std::string confirm_label;
    };

    struct confirmation {
        notification value;
        std::function<void(bool)> on_result;
        std::string confirm_label;
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
    void render_confirmation();

    std::mutex _mutex;
    std::deque<command> _pending;
    std::vector<toast> _toasts;
    std::deque<blocking_notification> _blocking;
    std::deque<notification> _errors;

    std::optional<notification> _active_error;
    std::optional<confirmation> _confirmation;
    std::deque<confirmation> _confirmation_queue;

    notification_id _next_id = 1;
    bool _error_popup_open_requested = false;
};

} // namespace cas::gui
