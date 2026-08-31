#pragma once

#include <array>

namespace cas {

struct rule {

    std::array<bool, 9> birth{};
    std::array<bool, 9> survive{};

    bool next(bool alive, int neighbors) const {
        if (alive)
            return survive[neighbors];
        else
            return birth[neighbors];
    }
};

} //namepsace cas

