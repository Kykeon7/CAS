#pragma once

#include <vector>
#include <array>
#include <utility>

struct HexCell {
    bool alive = false;
    bool next_alive = false;
};

class HexAutomaton {
public:
    HexAutomaton(int width, int height);

    void randomize();
    void step();
    void toggle(int q, int r);
    bool in_bounds(int q, int r) const;

    const HexCell& cell(int q, int r) const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    int alive_count() const { return m_alive_count; }
    int generation_count() const { return m_generation_count; }

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

    std::vector<HexCell> m_cells;
};
