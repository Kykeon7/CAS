#pragma once

#include <files/dialog_service.h>
#include <files/serialization/automaton.h>
#include <files/serialization/progress.h>
#include <files/serialization/viewport.h>
#include <files/serialization/workspace.h>

#include <automata/automaton.h>

#include <condition_variable>
#include <deque>
#include <filesystem>
#include <memory>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include <viewports/manager.h>
#include <viewports/viewport.h>

namespace cas::files {

using loaded_file = std::variant<
    std::unique_ptr<automata::automaton>,
    std::unique_ptr<cas::viewports::viewport>,
    std::unique_ptr<serialization::workspace_data>
>;

enum class file_operation_action {
    completed,
    loaded_automaton,
    loaded_viewport,
    loaded_workspace,
    request_save_workspace_as,
    failed
};

using file_save_payload = std::variant<
    serialization::automaton_snapshot,
    serialization::viewport_snapshot,
    serialization::workspace_snapshot
>;

/// @brief Contains the data required to execute a file operation.
///
/// Progress callbacks are invoked from the worker thread and must be
/// safe to execute concurrently with the application thread.
struct file_job {
    file_job() = default;

    file_job(const file_job&) = default;
    file_job& operator=(const file_job&) = default;

    file_job(file_job&&) noexcept = default;
    file_job& operator=(file_job&&) noexcept = default;

    ~file_job() = default;

    files::file_operation operation{};
    std::filesystem::path path;
    std::optional<file_save_payload> save_payload;

    cas::viewports::manager::viewport_id target_viewport = 0;

    serialization::progress_callback on_progress;
};

/// @brief Contains the outcome of an executed file operation.
struct file_operation_result {
    file_operation_result() = default;

    file_operation_result(const file_operation_result&) = default;
    file_operation_result& operator=(const file_operation_result&) = default;

    file_operation_result(file_operation_result&&) noexcept = default;
    file_operation_result& operator=(file_operation_result&&) noexcept = default;

    ~file_operation_result() = default;

    files::file_operation operation{};
    std::filesystem::path path;

    file_operation_action action =
        file_operation_action::completed;

    std::optional<loaded_file> loaded;
    std::string error_message;

    cas::viewports::manager::viewport_id target_viewport = 0;
};

/// @brief Executes file operations asynchronously on a worker thread.
class operation_handler {
public:
    operation_handler();

    /// @brief Stops the worker and waits for pending jobs to finish.
    ///
    /// Queued jobs are processed before the worker thread exits.
    ~operation_handler();

    operation_handler(const operation_handler&) = delete;
    operation_handler& operator=(const operation_handler&) = delete;

    operation_handler(operation_handler&&) = delete;
    operation_handler& operator=(operation_handler&&) = delete;

    /// @brief Submits a file job for asynchronous execution.
    /// @param job Job to enqueue.
    void submit(file_job job);

    /// @brief Retrieves and removes all completed operation results.
    /// @return Completed results currently available.
    [[nodiscard]]
    std::vector<file_operation_result> consume_results();

    /// @brief Blocks until all submitted jobs have finished.
    ///
    /// Completed results remain available until consumed separately.
    void wait_until_idle();

private:
    void worker_loop(std::stop_token stop_token);

    [[nodiscard]]
    file_operation_result execute(file_job job);

    std::mutex _mutex;
    std::condition_variable_any _work_available;
    std::condition_variable _idle;

    std::deque<file_job> _jobs;
    std::deque<file_operation_result> _results;

    std::jthread _worker;
    bool _busy = false;
};

} // namespace cas::files
