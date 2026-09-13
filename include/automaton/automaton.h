#pragma once

#include "topology.h"
#include "rule.h"
#include <automaton/grid_storage.h>

namespace cas {

class automaton {
public:

    automaton(
        int width,
        int height,
        topology topology,
        rule rule
    )
        : m_storage(width, height),
        m_topology(topology),
        m_rule(rule)
    {
    }

    void step() {
        int W = m_storage.width();
        int H = m_storage.height();
        size_t cells = m_storage.size();

        int N = m_topology.neighbor_count();

        m_alive_count = 0;
        m_generation_count++;

        for (int i = 0; i < static_cast<int>(cells); i++) {
            bool alive = m_storage.get(i);

            int neighbors = 0;

            for (int n = 0; n < N; n++) {
                int idx = m_topology.neighbor_index(i, n, W, H);

                if (idx >= 0)
                    neighbors += m_storage.get(idx);
            }

            bool next = m_rule.next(alive, neighbors);

            m_storage.set_next(i, next);
            m_alive_count += next;
        }
    
        m_storage.backup();
        m_storage.swap_buffers();

    }

    void randomize() {
        m_alive_count = 0;
        m_generation_count = 0;

        auto size = static_cast<int>(m_storage.size());

        for (int i = 0; i < size; i++) {
            bool v = (std::rand() % 2) == 0;

            m_storage.set_next(i, v);
            m_alive_count += v;
        }

        m_storage.swap_buffers();
    }

    bool in_bounds(int q, int r) const {
        return q >= 0 && q < m_storage.width() &&
            r >= 0 && r < m_storage.height();
    }

    void toggle(int i) {
        m_storage.set(i, !m_storage.get(i)); 
    }

    void toggle(int x, int y) {
        int i = y * m_storage.width() + x;
        toggle(i);
    }

    bool get(int x, int y) const {
        return m_storage.get(y * m_storage.width() + x);
    }

    void set_rule(const rule& rule) noexcept {
        m_rule = rule;
    }

    void set_topology(const topology& topology) noexcept {
        m_topology = topology;
    }

    void resize(int width, int height) {
        m_storage.resize(width, height);
    }

    int width() const noexcept { return m_storage.width(); }
    int height() const noexcept { return m_storage.height(); }
    topology get_topology() const noexcept { return m_topology; }
    rule get_rule() const noexcept { return m_rule; }

    int alive_count() const noexcept { return m_alive_count; }
    int generation_count() const noexcept { return m_generation_count; }

private:
    grid_storage m_storage;
    cas::topology m_topology;
    cas::rule m_rule;
    int m_alive_count = 0;
    int m_generation_count = 0;
};

} // namespace cas

