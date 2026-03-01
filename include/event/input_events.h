#pragma once

namespace cas {

 struct KeyPressedEvent {
    int key;
    bool repeat;
 };

struct KeyReleasedEvent {
    int key;
};

struct TextInputEvent {
    char character;
};

struct MouseScrolledEvent {
    float offset;
};

} // namespace cas