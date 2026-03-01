#include "platform/sdl_context.h"

#include <stdexcept>

namespace cas {

    SDLContext::SDLContext(Uint32 flags)
        : m_flags(flags)
    {
        if (!SDL_Init(flags)) {
            throw std::runtime_error(SDL_GetError());
        }
    }

    SDLContext::~SDLContext() {
        SDL_Quit();
    }

}
