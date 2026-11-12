#pragma once

#include <nlohmann/json.hpp>

#include <filesystem>
#include <vector>

#include <automata/automaton.h>
#include <files/serialization/progress.h>

namespace cas::files::serialization {

/// @brief Stores an independent snapshot of an automaton.
/// @details Contains the automaton dimensions, generation, rule and
///          topology metadata, and the complete cell configuration.
struct automaton_snapshot {
    automaton_snapshot() = default;

    automaton_snapshot(const automaton_snapshot&) = default;
    automaton_snapshot& operator=(const automaton_snapshot&) = default;

    automaton_snapshot(automaton_snapshot&&) = default;
    automaton_snapshot& operator=(automaton_snapshot&&) = default;

    ~automaton_snapshot() = default;

    /// @brief Number of columns in the automaton.
    int width{ 0 };

    /// @brief Number of rows in the automaton.
    int height{ 0 };

    /// @brief Current generation number.
    int generation{ 0 };

    /// @brief Serialized rule metadata.
    nlohmann::json rule;

    /// @brief Serialized topology metadata.
    nlohmann::json topology;

    /// @brief Cell states in row-major order.
    std::vector<automata::cell_state> cells;
};

/// @brief Captures an independent snapshot of an automaton.
/// @param value Automaton to capture.
/// @return Snapshot containing its dimensions, metadata, and cell states.
[[nodiscard]]
automaton_snapshot capture_automaton(
    const automata::automaton& value
);

/// @brief Serializes an automaton snapshot into a JSON document.
/// @param snapshot Snapshot to serialize.
/// @return JSON representation of the snapshot.
[[nodiscard]]
nlohmann::json save_automaton(
    const automaton_snapshot& snapshot
);

/// @brief Captures and serializes an automaton into a JSON document.
/// @param value Automaton to serialize.
/// @return JSON representation of the automaton.
[[nodiscard]]
nlohmann::json save_automaton(
    const automata::automaton& value
);

/// @brief Reconstructs an automaton from a JSON document.
/// @param document JSON document containing the automaton data.
/// @param progress Optional progress reporter.
/// @return Reconstructed automaton.
[[nodiscard]]
automata::automaton load_automaton(
    const nlohmann::json& document,
    progress_reporter progress = {}
);

/// @brief Reads and reconstructs an automaton from a file.
/// @param path Path to the JSON file.
/// @param on_progress Optional progress callback.
/// @return Reconstructed automaton.
[[nodiscard]]
automata::automaton load_automaton_file(
    const std::filesystem::path& path,
    progress_callback on_progress = {}
);

/// @brief Serializes an automaton and writes it to a file.
/// @param path Destination file path.
/// @param value Automaton to serialize.
void save_automaton_file(
    const std::filesystem::path& path,
    const automata::automaton& value
);

/// @brief Writes an automaton snapshot to a file.
/// @param path Destination file path.
/// @param snapshot Snapshot to serialize.
void save_automaton_file(
    const std::filesystem::path& path,
    const automaton_snapshot& snapshot
);

} // namespace cas::files::serialization