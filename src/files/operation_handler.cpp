#include <files/operation_handler.h>

#include <stdexcept>
#include <utility>

namespace cas::files {

operation_handler::operation_handler() :
    _worker(
        [this](std::stop_token stop_token) {
            worker_loop(stop_token);
        }
    )
{
}

operation_handler::~operation_handler() {
    _worker.request_stop();
    _work_available.notify_all();

    if (_worker.joinable()) 
        _worker.join();
}

void operation_handler::submit(file_job job) {
    {
        std::lock_guard lock(_mutex);
        _jobs.push_back(std::move(job));
    }

    _work_available.notify_one();
}

std::vector<file_operation_result>
operation_handler::consume_results() {
    std::vector<file_operation_result> results;

    {
        std::lock_guard lock(_mutex);

        results.reserve(_results.size());

        while (!_results.empty()) {
            results.push_back(std::move(_results.front()));
            _results.pop_front();
        }
    }

    return results;
}

void operation_handler::wait_until_idle() {
    std::unique_lock lock(_mutex);

    _idle.wait(lock, [this] {
        return _jobs.empty() && !_busy;
    });
}

void operation_handler::worker_loop(
    std::stop_token stop_token
) {
    while (true) {
        file_job job;

        {
            std::unique_lock lock(_mutex);

            _work_available.wait(
                lock,
                stop_token,
                [this] {
                    return !_jobs.empty();
                }
            );

            // On shutdown, finish queued jobs before exiting.
            if (_jobs.empty()) {
                if (stop_token.stop_requested())
                    return;

                continue;
            }

            job = std::move(_jobs.front());
            _jobs.pop_front();
            _busy = true;
        }

        file_operation_result result = execute(
            std::move(job)
        );

        {
            std::lock_guard lock(_mutex);

            _results.push_back(std::move(result));
            _busy = false;

            if (_jobs.empty())
                _idle.notify_all();
        }
    }
}

file_operation_result operation_handler::execute(
    file_job job
) {
    file_operation_result result;
    result.operation = job.operation;
    result.path = job.path;
    result.target_viewport = job.target_viewport;

    try {
        switch (job.operation) {
        case files::file_operation::save_automaton: {
            if (!job.save_payload.has_value())
                throw std::runtime_error(
                    "Missing automaton save snapshot"
                );

            const auto* snapshot =
                std::get_if<
                serialization::automaton_snapshot
                >(&*job.save_payload);

            if (snapshot == nullptr)
                throw std::runtime_error(
                    "Invalid payload for automaton save"
                );

            serialization::save_automaton_file(
                job.path,
                *snapshot
            );

            result.action =
                file_operation_action::completed;
            break;
        }

        case files::file_operation::load_automaton: {
            result.loaded.emplace(
                std::make_unique<automata::automaton>(
                    serialization::load_automaton_file(
                        job.path,
                        job.on_progress
                    )
                )
            );

            result.action =
                file_operation_action::loaded_automaton;
            break;
        }

        case files::file_operation::save_viewport: {
            if (!job.save_payload.has_value())
                throw std::runtime_error(
                    "Missing viewport save snapshot"
                );

            const auto* snapshot =
                std::get_if<
                serialization::viewport_snapshot
                >(&*job.save_payload);

            if (snapshot == nullptr)
                throw std::runtime_error(
                    "Invalid payload for viewport save"
                );

            serialization::save_viewport_file(
                job.path,
                *snapshot
            );

            result.action =
                file_operation_action::completed;
            break;
        }

        case files::file_operation::load_viewport: {
            result.loaded.emplace(
                std::make_unique<cas::viewports::viewport>(
                    serialization::load_viewport_file(
                        job.path,
                        job.on_progress
                    )
                )
            );

            result.action =
                file_operation_action::loaded_viewport;
            break;
        }

        case files::file_operation::save_workspace:
        case files::file_operation::save_workspace_as: {
            if (!job.save_payload.has_value())
                throw std::runtime_error(
                    "Missing workspace save snapshot"
                );

            const auto* snapshot =
                std::get_if<
                serialization::workspace_snapshot
                >(&*job.save_payload);

            if (snapshot == nullptr)
                throw std::runtime_error(
                    "Invalid payload for workspace save"
                );

            serialization::save_workspace_file(
                job.path,
                *snapshot
            );

            result.action =
                file_operation_action::completed;
            break;
        }

        case files::file_operation::load_workspace: {
            result.loaded.emplace(
                std::make_unique<serialization::workspace_data>(
                    serialization::load_workspace_file(
                        job.path,
                        job.on_progress
                    )
                )
            );

            result.action =
                file_operation_action::loaded_workspace;
            break;
        }

        default:
            throw std::runtime_error(
                "Unsupported file operation"
            );
        }
    }
    catch (const std::exception& exception) {
        result.action = file_operation_action::failed;
        result.error_message = exception.what();
        result.loaded.reset();
    }
    catch (...) {
        result.action = file_operation_action::failed;
        result.error_message =
            "Unknown error during file operation";
        result.loaded.reset();
    }

    return result;
}

} // namespace cas::files
