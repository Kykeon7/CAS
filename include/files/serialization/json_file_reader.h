#pragma once

#include <nlohmann/json.hpp>

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>

#include <files/serialization/progress.h>

namespace cas::files::serialization {

/// @brief Reads and parses a JSON file.
/// @param path Path to the JSON file.
/// @param on_progress Optional callback invoked as bytes are read.
/// @return Parsed JSON document.
/// @throws std::runtime_error If the file cannot be accessed or read.
/// @throws nlohmann::json::parse_error If the file contains invalid JSON.
[[nodiscard]]
inline nlohmann::json read_json_file(
    const std::filesystem::path& path,
    const read_progress_callback& on_progress = {}
) {
    std::error_code error;

    const auto file_size =
        std::filesystem::file_size(path, error);

    if (error) {
        throw std::runtime_error(
            "Failed to determine file size: " + path.string()
        );
    }

    if (file_size > std::string{}.max_size()) {
        throw std::runtime_error(
            "File is too large to read: " + path.string()
        );
    }

    std::ifstream file(path, std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(
            "Failed to open file for reading: " + path.string()
        );
    }

    constexpr std::size_t buffer_size = 64 * 1024;

    std::vector<char> buffer(buffer_size);
    std::string contents;

    contents.reserve(static_cast<std::size_t>(file_size));

    const auto total = static_cast<std::uint64_t>(file_size);
    std::uint64_t completed = 0;

    if (on_progress)
        on_progress(completed, total);

    while (true) {
        file.read(
            buffer.data(),
            static_cast<std::streamsize>(buffer.size())
        );

        const auto bytes_read = file.gcount();

        if (bytes_read > 0) {
            contents.append(
                buffer.data(),
                static_cast<std::size_t>(bytes_read)
            );

            completed += static_cast<std::uint64_t>(bytes_read);

            if (on_progress)
                on_progress(completed, total);
        }

        if (file.bad()) {
            throw std::runtime_error(
                "Failed to read file: " + path.string()
            );
        }

        if (file.eof())
            break;

        if (file.fail()) {
            throw std::runtime_error(
                "Unexpected failure while reading file: "
                + path.string()
            );
        }
    }

    return nlohmann::json::parse(contents);
}

} // namespace cas::files::serialization