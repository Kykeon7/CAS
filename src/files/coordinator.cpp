
#include <files/coordinator.h>

#include <files/serialization/automaton.h>
#include <files/serialization/viewport.h>
#include <files/serialization/workspace.h>

#include <utility>

namespace cas::files {

coordinator::coordinator(
    dialog_service& dialog_service,
    operation_handler& operation_handler,
    workspace& workspace
)
    : _dialog_service(dialog_service),
      _operation_handler(operation_handler),
      _workspace(workspace) {
}

request_status coordinator::submit_request(
    const file_request& request,
    progress_callback_factory make_progress_callback
) {
    using serialization::capture_automaton;
    using serialization::capture_viewport;
    using serialization::capture_workspace;

    auto& viewports_manager = _workspace.viewports();

    const auto target_viewport = viewports_manager.active_id();
    const auto& viewports = viewports_manager.viewports();

    file_job job{
        .operation = request.operation,
        .path = request.path,
        .target_viewport = target_viewport
    };

    switch (request.operation) {
    case file_operation::save_automaton: {
        if (target_viewport >= viewports.size()
            || !viewports[target_viewport].has_value()) {
            return request_status::rejected_no_active_viewport;
        }

        const auto& active_viewport = *viewports[target_viewport];

        job.save_payload = file_save_payload{
            capture_automaton(
                active_viewport.simulation().automaton()
            )
        };

        break;
    }

    case file_operation::load_automaton:
        if (target_viewport >= viewports.size()
            || !viewports[target_viewport].has_value()) {
            return request_status::rejected_no_active_viewport;
        }

        break;

    case file_operation::save_viewport: {
        if (target_viewport >= viewports.size()
            || !viewports[target_viewport].has_value()) {
            return request_status::rejected_no_active_viewport;
        }

        const auto& active_viewport = *viewports[target_viewport];

        job.save_payload = file_save_payload{
            capture_viewport(active_viewport)
        };

        break;
    }

    case file_operation::load_viewport:
        if (target_viewport >= viewports.size()
            || !viewports[target_viewport].has_value()) {
            return request_status::rejected_no_active_viewport;
        }

        break;

    case file_operation::save_workspace:
        if (!_workspace.has_location()) {
            _dialog_service.open_dialog(
                file_operation::save_workspace_as
            );

            return request_status::redirected_to_save_as;
        }

        job.path = _workspace.path();

        job.save_payload = file_save_payload{
            capture_workspace(
                viewports_manager,
                _workspace.ui_scale()
            )
        };

        break;

    case file_operation::save_workspace_as:
        job.save_payload = file_save_payload{
            capture_workspace(
                viewports_manager,
                _workspace.ui_scale()
            )
        };

        break;

    case file_operation::load_workspace:
        break;
    case file_operation::select_save_directory:
        return request_status::handled_externally;
    }

    if (make_progress_callback) {
        switch (request.operation) {
        case file_operation::load_automaton:
        case file_operation::load_viewport:
        case file_operation::load_workspace:
            job.on_progress = make_progress_callback(
                request.operation
            );
            break;

        default:
            break;
        }
    }

    _operation_handler.submit(std::move(job));

    return request_status::submitted;
}

result_status coordinator::process_result(
    const file_operation_result& result
) {
    if (result.action == file_operation_action::request_save_workspace_as) {
        _dialog_service.open_dialog(
            file_operation::save_workspace_as
        );

        return result_status::save_as_requested;
    }

    if (result.action != file_operation_action::completed) {
        return result_status::not_handled;
    }

    switch (result.operation) {
    case file_operation::save_workspace:
    case file_operation::save_workspace_as:
        _workspace.set_location(result.path);
        return result_status::workspace_saved;

    default:
        return result_status::not_handled;
    }
}

} // namespace cas::files
