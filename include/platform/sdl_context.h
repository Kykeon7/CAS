#pragma once

#include <SDL3/SDL.h>

namespace cas::platform {

/**
 * @brief Manages the initialization and shutdown of SDL.
 *
 * Initializes SDL with the specified subsystem flags upon construction
 * and calls SDL_Quit() upon destruction.
 *
 * The context owns the SDL initialization lifecycle and should outlive
 * the objects that depend on SDL.
 */
class sdl_context {
public:
    /**
     * Initializes SDL with the specified subsystem flags.
     *
     * @param flags SDL subsystem initialization flags.
     * @throws std::runtime_error If SDL initialization fails.
     */
    explicit sdl_context(Uint32 flags);

    /** Shuts down SDL and releases its initialized subsystems. */
    ~sdl_context();

    sdl_context(const sdl_context&) = delete;
    sdl_context& operator=(const sdl_context&) = delete;

    sdl_context(sdl_context&&) = delete;
    sdl_context& operator=(sdl_context&&) = delete;

private:
    Uint32 _flags;
};

} // namespace cas::platform