#pragma once

#include <files/dialog_service.h>
#include <files/operation_handler.h>
#include <workspace/workspace.h>

#include <functional>

namespace cas::files {

enum class request_status {
    submitted,
    rejected_no_active_viewport,
    redirected_to_save_as,
    handled_externally
};

enum class result_status {
    not_handled,
    workspace_saved,
    save_as_requested
};

class coordinator {
public:
    using progress_callback_factory =
        std::function<serialization::progress_callback(file_operation)>;

    coordinator(
        dialog_service& dialog_service,
        operation_handler& operation_handler,
        workspace& workspace
    );

    [[nodiscard]]
    request_status submit_request(
        const file_request& request,
        progress_callback_factory make_progress_callback = {}
    );

    [[nodiscard]]
    result_status process_result(
        const file_operation_result& result
    );

private:
    dialog_service& _dialog_service;
    operation_handler& _operation_handler;
    workspace& _workspace;
};

} // namespace cas::files
