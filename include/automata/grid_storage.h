#pragma once

#include "cell_state.h"

#include <cstddef>
#include <vector>

namespace cas::automata {

class grid_storage {
public:
    grid_storage(int width, int height);

    cell_state get(int i) const;
    cell_state get_next(int i) const;

    void set_next(int i, cell_state state);

    void swap_buffers();

    void set(size_t i, cell_state state);

    void resize(int width, int height);

    size_t size() const;

    int width() const;
    int height() const;

    const std::vector<cell_state>& current() const;
    std::vector<cell_state>& next();

private:
    int _width;
    int _height;

    std::vector<cell_state> _current;
    std::vector<cell_state> _next;
};

} // namespace cas::automata