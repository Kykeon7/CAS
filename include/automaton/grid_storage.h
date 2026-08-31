#pragma once

#include "bit_array.h"

namespace cas {

class grid_storage {
public:
    grid_storage(int width, int height)
        : m_width(width)
        , m_height(height)
        , m_current(width * height)
        , m_next(width * height)
        , m_previous(width * height) {
    }

    bool get(int i) const { return m_current.get(i); }
    bool get_next(int i) const { return m_next.get(i); }

    void set_next(int i, bool v) { m_next.set(i, v); }

    void swap_buffers() {
        std::swap(m_current, m_next);
    }

    void set(size_t i, bool v) {
        m_current.set(i, v);
    }

    void resize(int width, int height) {
        m_width = width;
        m_height = height;
            
        int size = width * height;

        bit_array empty(size);

        m_current = empty;
        m_next = empty;
    }

    size_t size() const { return static_cast<size_t>(m_width) * m_height; }

    int width() const { return m_width; }
    int height() const { return m_height; }

    void backup() {
        m_previous = m_current;
    }

    const bit_array& current() const { return m_current; }
    bit_array& previous() { return m_previous; }
    bit_array& next() { return m_next; }

private:
    int m_width;
    int m_height;
  
    bit_array m_previous;
    bit_array m_current;
    bit_array m_next;
};

} // namespace cas