#pragma once

#include <SDL3/SDL.h>

namespace cas {

class SDLContext {
public:
	explicit SDLContext(Uint32 flags);
	~SDLContext();

	SDLContext(const SDLContext&) = delete;
	SDLContext& operator=(const SDLContext&) = delete;
private:
	Uint32 m_flags;
};

}
