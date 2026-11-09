
#pragma once

#include <files/serialization/automaton.h>
#include <files/serialization/viewport.h>
#include <files/serialization/workspace.h>

#include <automata/automaton.h>
#include <viewports/viewport.h>

#include <condition_variable>
#include <deque>
#include <filesystem>
#include <mutex>
#include <optional>
#include <stop_token>
#include <string>
#include <thread>
#include <variant>
#include <vector>
#include <memory>
#include <files/dialog_service.h>
#include <files/serialization/progress.h>

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

struct file_job {
    files::file_operation operation{};
    std::filesystem::path path;
    std::optional<file_save_payload> save_payload;

    cas::viewports::manager::viewport_id target_viewport = 0;

    serialization::progress_callback on_progress;
};

struct file_operation_result {
    files::file_operation operation{};
    std::filesystem::path path;

    file_operation_action action =
        file_operation_action::completed;

    std::optional<loaded_file> loaded;
    std::string error_message;

    cas::viewports::manager::viewport_id target_viewport = 0;
};

class operation_handler {
public:
    operation_handler();
    ~operation_handler();

    operation_handler(
        const operation_handler&
    ) = delete;

    operation_handler& operator=(
        const operation_handler&
    ) = delete;

    void submit(file_job job);

    std::vector<file_operation_result> consume_results();

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

} // namespace cas
