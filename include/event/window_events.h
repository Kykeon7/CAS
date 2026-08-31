#pragma once

namespace cas::event {

struct WindowResized {
    int width;
    int height;
};

struct WindowFullScreenToggle {};

struct Quit {};

}