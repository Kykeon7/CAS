#pragma once

#include "core/math/vec2.h"

namespace cas::event {

 struct KeyPressed {
    int key;
    bool repeat;
 };

struct KeyReleased {
    int key;
};

struct TextInput {
    char character;
};

struct MouseScrolled {
    float offset;
};

struct MoveCamera {
    Vec2 delta;
};

struct ZoomCamera {
    float factor;
    Vec2 mouse_screen_pos;
};

struct CellClicked {
    Vec2 screen_pos;
};


} // namespace cas