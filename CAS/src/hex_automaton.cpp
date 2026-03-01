#include "automaton/hex_automaton.h"
#include <cstdlib>

HexAutomaton::HexAutomaton(int width, int height)
    : m_width(width)
    , m_height(height)
    , m_cells(width* height) {
}

int HexAutomaton::index(int q, int r) const {
    return r * m_width + q;
}

const HexCell& HexAutomaton::cell(int q, int r) const {
    return m_cells[index(q, r)];
}

void HexAutomaton::randomize() {
    for (auto& cell : m_cells)
        cell.alive = (std::rand() % 2) == 0;

    m_generation_count = 0;
}

int HexAutomaton::alive_neighbors(int q, int r) const {
    int count = 0;

    for (const auto& [dq, dr] : directions) {
        int nq = q + dq;
        int nr = r + dr;

        if (nq >= 0 && nq < m_width &&
            nr >= 0 && nr < m_height) {
            if (m_cells[index(nq, nr)].alive)
                ++count;
        }
    }

    return count;
}

void HexAutomaton::step() {
    m_alive_count = 0;
    m_generation_count++;

    // Primera pasada: calcular siguiente estado
    for (int r = 0; r < m_height; ++r) {
        for (int q = 0; q < m_width; ++q) {
            auto& cell = m_cells[index(q, r)];
            int neighbors = alive_neighbors(q, r);

            if (cell.alive)
                cell.next_alive = (neighbors == 2 || neighbors == 3);
            else
                cell.next_alive = (neighbors == 3);

            m_alive_count += cell.next_alive;
        }
    }


    // Segunda pasada: aplicar
    for (auto& cell : m_cells)
        cell.alive = cell.next_alive;
}

void HexAutomaton::toggle(int q, int r) {
    auto& cell = m_cells[index(q, r)];
    cell.alive = !cell.alive;
}

bool HexAutomaton::in_bounds(int q, int r) const {
    return q >= 0 && q < m_width &&
        r >= 0 && r < m_height;
}
