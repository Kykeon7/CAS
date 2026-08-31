#pragma once

#include <core/viewport.h>

namespace cas::panels {

class Panel {
public: 
	virtual void render() = 0;

	virtual void set_viewport(Viewport&) {}

	virtual ~Panel() = default;
};

} // namespace cas::panels