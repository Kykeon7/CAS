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
    Q,
    space,
    escape,
    F11,
    left_ctrl,
    right_ctrl,
    left_alt,
    right_alt,
    left_shift,
    right_shift
};

/** Represents the mouse buttons supported by the input system. */
enum class mouse_button {
    left,
    right,
    middle
};

} // namespace cas::inputs