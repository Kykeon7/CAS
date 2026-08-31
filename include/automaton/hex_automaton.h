#pragma once

#include <vector>
#include <array>
#include <utility>
#include "bit_array.h"

struct hex_cell {
    bool alive = false;
    bool next_alive = false;
};

class hex_automaton {
public:
    hex_automaton(int width, int height);

    void randomize();
    void step();
    void toggle(int q, int r);
    bool in_bounds(int q, int r) const;

    const hex_cell& cell(int q, int r) const;

    int width() const noexcept { return m_width; }
    int height() const noexcept { return m_height; }
    int alive_count() const noexcept { return m_alive_count; }
    int generation_count() const noexcept { return m_generation_count; }

private:
    int index(int q, int r) const;
    int alive_neighbors(int q, int r) const;

private:
    static constexpr std::array<std::pair<int, int>, 6> directions{ {
        { +1,  0 },
        { -1,  0 },
        {  0, +1 },
        {  0, -1 },
        { +1, -1 },
        { -1, +1 }
    } };

    int m_width;
    int m_height;
    int m_alive_count = 0;
    int m_generation_count = 0;

    std::vector<hex_cell> m_cells;
};
