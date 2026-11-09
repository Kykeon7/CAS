
#include <files/serialization/workspace.h>

#include <nlohmann/json.hpp>

#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <cstdint>
#include <vector>
#include <files/serialization/json_file_reader.h>

namespace cas::files::serialization {
namespace {

using json = nlohmann::json;
using viewport_id = cas::viewports::manager::viewport_id;

constexpr int schema_version = 1;

std::string id_to_string(viewport_id id) {
    return std::to_string(id);
}

viewport_id parse_viewport_id(std::string_view key) {
    if (key.empty()) {
        throw std::runtime_error(
            "Empty viewport ID"
        );
    }

    viewport_id id = 0;

    for (const char character : key) {
        if (character < '0' || character > '9') {
            throw std::runtime_error(
                "Invalid viewport ID"
            );
        }

        const auto digit =
            static_cast<viewport_id>(character - '0');

        if (id > (std::numeric_limits<viewport_id>::max()
                  - digit) / 10) {
            throw std::runtime_error(
                "Viewport ID overflow"
            );
        }

        id = id * 10 + digit;
    }

    if (id >= cas::viewports::manager::max_viewports) {
        throw std::runtime_error(
            "Viewport ID out of range"
        );
    }

    return id;
}

void validate_ui_scale(float ui_scale) {
    if (!std::isfinite(ui_scale) || ui_scale <= 0.0f) {
        throw std::runtime_error(
            "Invalid UI scale"
        );
    }
}

void validate_active_viewport(
    viewport_id active_viewport,
    const std::array<
        std::optional<viewport_snapshot>,
        cas::viewports::manager::max_viewports
    >& viewports
) {
    if (active_viewport >= viewports.size()
        || !viewports[active_viewport].has_value()) {
        throw std::runtime_error(
            "Active viewport is missing from workspace"
        );
    }
}

std::uint64_t get_viewport_cell_count(
    const json& document
) {
    const auto& simulation = document.at("simulation");

    const int width = simulation.at("width").get<int>();
    const int height = simulation.at("height").get<int>();

    if (width <= 0 || height <= 0) {
        throw std::runtime_error(
            "Invalid viewport dimensions"
        );
    }

    const auto cell_count =
        static_cast<std::int64_t>(width) * height;

    if (cell_count > std::numeric_limits<int>::max()) {
        throw std::runtime_error(
            "Viewport dimensions exceed the supported cell count"
        );
    }

    return static_cast<std::uint64_t>(cell_count);
}

} // namespace

workspace_snapshot capture_workspace(
    const cas::viewports::manager& viewports,
    float ui_scale
) {
    validate_ui_scale(ui_scale);

    workspace_snapshot snapshot;
    snapshot.active_viewport = viewports.active_id();
    snapshot.ui_scale = ui_scale;

    const auto& slots = viewports.viewports();

    if (snapshot.active_viewport >= slots.size()
        || !slots[snapshot.active_viewport].has_value()) {
        throw std::runtime_error(
            "Active viewport is missing from workspace"
        );
    }

    for (viewport_id id = 0; id < slots.size(); ++id) {
        if (!slots[id].has_value()) {
            continue;
        }

        snapshot.viewports[id] =
            capture_viewport(*slots[id]);
    }

    return snapshot;
}

json save_workspace(const workspace_snapshot& snapshot) {
    validate_ui_scale(snapshot.ui_scale);

    validate_active_viewport(
        snapshot.active_viewport,
        snapshot.viewports
    );

    json serialized_viewports = json::object();

    for (viewport_id id = 0;
         id < snapshot.viewports.size();
         ++id) {
        if (!snapshot.viewports[id].has_value()) {
            continue;
        }

        serialized_viewports[id_to_string(id)] =
            save_viewport(*snapshot.viewports[id]);
    }

    return {
        { "schema_version", schema_version },
        { "ui_scale", snapshot.ui_scale },
        { "active_viewport", snapshot.active_viewport },
        { "viewports", std::move(serialized_viewports) }
    };
}

json save_workspace(
    const cas::viewports::manager& viewports,
    float ui_scale
) {
    return save_workspace(
        capture_workspace(viewports, ui_scale)
    );
}


workspace_data load_workspace(
    const json& document,
    progress_reporter progress
) {
    if (!document.is_object()) {
        throw std::runtime_error(
            "Workspace document must be an object"
        );
    }

    progress.report(
        0,
        1,
        "Validating workspace metadata"
    );

    if (document.at("schema_version").get<int>()
        != schema_version) {
        throw std::runtime_error(
            "Unsupported workspace schema version"
        );
    }

    const float new_ui_scale =
        document.at("ui_scale").get<float>();

    validate_ui_scale(new_ui_scale);

    const auto& serialized_viewports =
        document.at("viewports");

    if (!serialized_viewports.is_object()
        || serialized_viewports.empty()) {
        throw std::runtime_error(
            "Workspace must contain at least one viewport"
        );
    }

    std::array<
        const json*,
        cas::viewports::manager::max_viewports
    > documents{};

    for (auto it = serialized_viewports.begin();
        it != serialized_viewports.end();
        ++it) {
        const viewport_id id =
            parse_viewport_id(it.key());

        if (documents[id] != nullptr) {
            throw std::runtime_error(
                "Duplicate viewport ID"
            );
        }

        documents[id] = &it.value();
    }

    const auto new_active_id =
        document.at("active_viewport").get<viewport_id>();

    if (new_active_id >= cas::viewports::manager::max_viewports
        || documents[new_active_id] == nullptr) {
        throw std::runtime_error(
            "Active viewport is missing from workspace"
        );
    }

    std::array<
        std::uint64_t,
        cas::viewports::manager::max_viewports
    > cell_counts{};

    std::uint64_t total_cells = 0;

    for (viewport_id id = 0;
        id < documents.size();
        ++id) {
        if (documents[id] == nullptr)
            continue;

        cell_counts[id] =
            get_viewport_cell_count(*documents[id]);

        total_cells += cell_counts[id];
    }

    if (total_cells == 0) {
        throw std::runtime_error(
            "Workspace contains no cells to restore"
        );
    }

    // Adapt a local 1000-unit progress range to the
    // progress range received from the caller.
    progress_reporter viewport_work(
        [&progress](
            std::uint64_t completed,
            std::uint64_t total,
            std::string_view stage
            ) {
                progress.report(
                    completed,
                    total,
                    stage
                );
        },
        1000
    );

    std::uint64_t completed_cells = 0;

    workspace_data result;
    result.ui_scale = new_ui_scale;

    auto& slots = result.viewports.viewports();

    for (auto& slot : slots) {
        slot.reset();
    }

    for (viewport_id id = 0;
        id < cas::viewports::manager::max_viewports;
        ++id) {
        if (documents[id] == nullptr)
            continue;

        const auto cell_count = cell_counts[id];

        const auto begin =
            completed_cells * 1000 / total_cells;

        completed_cells += cell_count;

        const auto end =
            completed_cells * 1000 / total_cells;

        const auto viewport_progress =
            viewport_work.subrange(
                begin,
                end - begin
            );

        slots[id].emplace(
            load_viewport(
                *documents[id],
                viewport_progress
            )
        );
    }

    if (result.viewports.active_id() != new_active_id) {
        result.viewports.set_active(new_active_id);
    }

    progress.complete("Workspace restored");

    return result;
}

void save_workspace_file(
    const std::filesystem::path& path,
    const workspace_snapshot& snapshot
) {
    const json document = save_workspace(snapshot);

    std::ofstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Failed to open workspace file for writing: "
            + path.string()
        );
    }

    file << document.dump(4) << '\n';

    if (!file) {
        throw std::runtime_error(
            "Failed to write workspace file: "
            + path.string()
        );
    }
}

void save_workspace_file(
    const std::filesystem::path& path,
    const cas::viewports::manager& viewports,
    float ui_scale
) {
    save_workspace_file(
        path,
        capture_workspace(viewports, ui_scale)
    );
}



workspace_data load_workspace_file(
    const std::filesystem::path& path,
    progress_callback on_progress
) {
    progress_reporter progress(
        std::move(on_progress),
        1000
    );

    constexpr std::uint64_t read_weight = 150;
    constexpr std::uint64_t deserialize_weight = 850;

    const auto read_progress = progress.subrange(
        0,
        read_weight
    );

    const auto deserialize_progress = progress.subrange(
        read_weight,
        deserialize_weight
    );

    const auto document = read_json_file(
        path,
        [&read_progress](
            std::uint64_t completed,
            std::uint64_t total
            ) {
                read_progress.report(
                    completed,
                    total,
                    "Reading workspace file"
                );
        }
    );

    read_progress.complete("Workspace file read");

    auto result = load_workspace(
        document,
        deserialize_progress
    );

    progress.complete("Workspace loaded");

    return result;
}

} // namespace cas::files::serialization
