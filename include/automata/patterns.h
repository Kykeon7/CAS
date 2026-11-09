#pragma once

#include <automaton/pattern.h>

namespace cas::automata::patterns {

inline pattern block() {
    return pattern({
        {0, 0},
        {1, 0},
        {0, 1},
        {1, 1}
        });
}

inline pattern blinker() {
    return pattern({
        {-1, 0},
        { 0, 0},
        { 1, 0}
        });
}

inline pattern beehive() {
    return pattern({
        { 1, 0},
        { 2, 1},
        { 1, 2},
        { 0, 2},
        {-1, 1},
        { 0, 0}
        });
}

inline pattern glider() {
    return pattern({
        { 1, 0},
        { 2, 1},
        { 0, 2},
        { 1, 2},
        { 2, 2}
        });
}

inline pattern spiral() {
    return pattern({
        // ...
        });
}

} // namespace cas::automata