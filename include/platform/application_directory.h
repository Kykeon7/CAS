#pragma once

#include <SDL3/SDL.h>

#include <filesystem>
#include <stdexcept>
#include <string>

namespace cas::platform {

    /**
     * @brief Retrieves the directory containing the application executable.
     *
     * Uses SDL to obtain the executable's base path.
     *
     * @return Filesystem path to the application directory.
     *
     * @throws std::runtime_error If SDL fails to retrieve the base path.
     */
    inline std::filesystem::path get_application_directory() {
        const char* base_path = SDL_GetBasePath();

        if (base_path == nullptr) {
            throw std::runtime_error(
                "Failed to retrieve application directory: "
                + std::string(SDL_GetError())
            );
        }

        return std::filesystem::path(base_path);
    }

} // namespace cas::platform