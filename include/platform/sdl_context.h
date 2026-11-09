#pragma once

#include <SDL3/SDL.h>

namespace cas::platform {

class sdl_context {
public:
	explicit sdl_context(Uint32 flags);
	~sdl_context();

	sdl_context(const sdl_context&) = delete;
	sdl_context& operator=(const sdl_context&) = delete;
private:
	Uint32 _flags;
};

} // namespace cas::platform
