#pragma once

namespace cas::inputs {

/** Represents the keyboard keys supported by the input system. */
enum class key {
    unknown,
    W,
    A,
    S,
    D,
    R,
    space,
    escape,
    F11
};

/** Represents the mouse buttons supported by the input system. */
enum class mouse_button {
    left,
    right,
    middle
};

} // namespace cas::inputs