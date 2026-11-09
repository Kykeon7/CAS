#include "platform/sdl_context.h"

#include <stdexcept>

namespace cas::platform {

sdl_context::sdl_context(Uint32 flags)
    : _flags(flags)
{
    if (!SDL_Init(flags)) {
        throw std::runtime_error(SDL_GetError());
    }
}

sdl_context::~sdl_context() {
    SDL_Quit();
}

} // namespace cas::platform
