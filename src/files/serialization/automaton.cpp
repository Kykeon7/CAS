#include <files/serialization/automaton.h>

#include <automata/rules/binary.h>
#include <automata/rules/beehive.h>
#include <automata/rules/spiral.h>

#include <automata/rules/rule_22_majority_memory.h>
#include <automata/topology.h>
#include <array>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <files/serialization/json_file_reader.h>

namespace cas::files::serialization {

using json = nlohmann::json;

namespace {

json save_rule(const automata::rules::rule& rule) {
    if (const auto* binary =
        dynamic_cast<const automata::rules::binary*>(&rule)) {
        return {
            { "type", "binary" },
            { "birth", binary->birth() },
            { "survive", binary->survive() }
        };
    }

    if (dynamic_cast<const automata::rules::spiral*>(&rule)) {
        return {
            { "type", "spiral" }
        };
    }

    if (dynamic_cast<const automata::rules::beehive*>(&rule)) {
        return {
            { "type", "beehive" }
        };
    }

    if (dynamic_cast<const automata::rules::rule_22_majority_memory*>(&rule)) {
        return {
            { "type", "rule_22_majority_memory" }
        };
    }

    throw std::runtime_error(
        "Cannot serialize an unknown rule type."
    );
}

std::unique_ptr<automata::rules::rule> load_rule(const json& document) {
    const std::string type =
        document.at("type").get<std::string>();

    if (type == "spiral") {
        return std::make_unique<automata::rules::spiral>();
    }

    if (type == "beehive") {
        return std::make_unique<automata::rules::beehive>();
    }

    if (type == "rule_22_majority_memory") {
        return std::make_unique<automata::rules::rule_22_majority_memory>();
    }

    if (type == "binary") {
        const auto birth_values =
            document.at("birth").get<std::array<bool, 9>>();

        const auto survive_values =
            document.at("survive").get<std::array<bool, 9>>();

        return std::make_unique<automata::rules::binary>(
            birth_values,
            survive_values
        );
    }

    throw std::runtime_error(
        "Unknown rule type: " + type
    );
}   


json save_topology(const automata::topology& topology) {
    json directions = json::array();

    for (const auto& [x, y] : topology.directions()) {
        directions.push_back({ x, y });
    }

    const char* boundary =
        topology.boundary() == automata::boundary_type::finite
            ? "finite"
            : "toroidal";

    return {
        { "boundary", boundary },
        { "directions", std::move(directions) }
    };
}

automata::topology load_topology(const json& document) {
    const std::string boundary_name =
        document.at("boundary").get<std::string>();

    automata::boundary_type boundary;

    if (boundary_name == "finite") {
        boundary = automata::boundary_type::finite;
    }
    else if (boundary_name == "toroidal") {
        boundary = automata::boundary_type::toroidal;
    }
    else {
        throw std::runtime_error(
            "Unknown topology boundary: " + boundary_name
        );
    }

    const auto& values = document.at("directions");

    if (!values.is_array() || values.empty()) {
        throw std::runtime_error(
            "Topology must contain at least one direction."
        );
    }

    std::vector<std::pair<int, int>> directions;
    directions.reserve(values.size());

    for (const auto& value : values) {
        if (!value.is_array() || value.size() != 2) {
            throw std::runtime_error(
                "Invalid topology direction."
            );
        }

        directions.emplace_back(
            value.at(0).get<int>(),
            value.at(1).get<int>()
        );
    }

    return automata::topology(
        std::move(directions),
        boundary
    );
}

} // namespace


automaton_snapshot capture_automaton(
    const automata::automaton& value
) {
    const auto& storage = value.storage();

    automaton_snapshot snapshot;
    snapshot.width = value.width();
    snapshot.height = value.height();
    snapshot.generation = value.generation_count();

    // Serialize small metadata while the live automaton
    // is accessed from the main thread.
    snapshot.rule = save_rule(value.rule());
    snapshot.topology = save_topology(value.topology());

    // Own the cell data before handing the snapshot to
    // the worker thread.
    snapshot.cells.assign(
        storage.current().begin(),
        storage.current().end()
    );

    return snapshot;
}

json save_automaton(
    const automaton_snapshot& snapshot
) {
    json cells = json::array();
    cells.get_ref<json::array_t&>().reserve(
        snapshot.cells.size()
    );

    for (const auto state : snapshot.cells) {
        cells.push_back(static_cast<int>(state));
    }

    return {
        { "schema_version", 1 },
        { "width", snapshot.width },
        { "height", snapshot.height },
        { "generation", snapshot.generation },
        { "rule", snapshot.rule },
        { "topology", snapshot.topology },
        { "cells", std::move(cells) }
    };
}

json save_automaton(const automata::automaton& value) {
    return save_automaton(capture_automaton(value));
}


automata::automaton load_automaton(
    const json& document,
    progress_reporter progress
) {
    if (!document.is_object()) {
        throw std::runtime_error(
            "Automaton document must be a JSON object."
        );
    }

    if (document.at("schema_version").get<int>() != 1) {
        throw std::runtime_error(
            "Unsupported automaton document version."
        );
    }

    const int width = document.at("width").get<int>();
    const int height = document.at("height").get<int>();
    const int generation = document.at("generation").get<int>();

    if (width <= 0 || height <= 0 || generation < 0) {
        throw std::runtime_error(
            "Invalid automaton dimensions or generation."
        );
    }

    const auto cell_count =
        static_cast<std::int64_t>(width) * height;

    if (cell_count > std::numeric_limits<int>::max()) {
        throw std::runtime_error(
            "Automaton dimensions exceed the supported cell count."
        );
    }

    auto rule = load_rule(document.at("rule"));
    auto topology = load_topology(document.at("topology"));

    const auto& cell_values = document.at("cells");

    if (!cell_values.is_array() ||
        cell_values.size() != static_cast<std::size_t>(cell_count)) {
        throw std::runtime_error(
            "Cell data does not match automaton dimensions."
        );
    }

    automata::automaton restored(
        width,
        height,
        std::move(topology),
        std::move(rule)
    );

    std::vector<automata::cell_state> cells;
    cells.reserve(cell_values.size());

    constexpr std::size_t progress_interval = 4096;

    const auto total = static_cast<std::uint64_t>(
        cell_values.size()
        );

    std::uint64_t completed = 0;

    for (const auto& value : cell_values) {
        if (!value.is_number_integer()) {
            throw std::runtime_error(
                "Cell states must be integers."
            );
        }

        const int state = value.get<int>();

        if (state < 0 ||
            state >= restored.rule().state_count()) {
            throw std::runtime_error(
                "Cell state is not supported by the selected rule."
            );
        }

        cells.push_back(
            static_cast<automata::cell_state>(state)
        );

        ++completed;

        if (completed % progress_interval == 0
            || completed == total) {
            progress.report(
                completed,
                total,
                "Reconstructing automaton cells"
            );
        }
    }

    progress.report(
        total,
        total,
        "Restoring automaton state"
    );

    restored.restore_state(cells, generation);

    progress.complete("Automaton restored");

    return restored;
}



void save_automaton_file(
    const std::filesystem::path& path,
    const automaton_snapshot& snapshot
) {
    std::ofstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Could not open file for writing: " + path.string()
        );
    }

    file << save_automaton(snapshot).dump(2) << '\n';

    if (!file) {
        throw std::runtime_error(
            "Failed to write automaton file: " + path.string()
        );
    }
}

void save_automaton_file(
    const std::filesystem::path& path,
    const automata::automaton& value
) {
    save_automaton_file(path, capture_automaton(value));
}


automata::automaton load_automaton_file(
    const std::filesystem::path& path,
    progress_callback on_progress
) {
    progress_reporter progress(
        std::move(on_progress),
        1000
    );

    constexpr std::uint64_t read_weight = 200;
    constexpr std::uint64_t deserialize_weight = 800;

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
                    "Reading automaton file"
                );
        }
    );

    read_progress.complete("Automaton file read");

    auto result = load_automaton(
        document,
        deserialize_progress
    );

    progress.complete("Automaton loaded");

    return result;
}

} // namespace cas::files::serialization