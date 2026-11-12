#pragma once

#include <cstddef>
#include <vector>

#include <automata/cell_state.h>

namespace cas::automata {

/// @brief Stores the cell states of a rectangular cellular automaton grid.
/// @details Maintains two separate buffers: one for the current configuration
///          and another for the next configuration. The buffers can be
///          exchanged after a simulation step to advance the automaton
///          without copying the entire configuration.
class grid_storage {
public:
    /// @brief Constructs a grid storage with the specified dimensions.
    /// @param width Number of columns.
    /// @param height Number of rows.
    grid_storage(int width, int height);

    grid_storage(const grid_storage&) = default;
    grid_storage& operator=(const grid_storage&) = default;

    grid_storage(grid_storage&&) noexcept = default;
    grid_storage& operator=(grid_storage&&) noexcept = default;

    ~grid_storage() = default;

    /// @brief Returns the state of a cell in the current configuration.
    /// @param i Linear index of the cell.
    /// @return State of the requested cell.
    /// @pre The index must be within the storage bounds.
    cell_state get(int i) const {
        return _current[i];
    }

    /// @brief Returns the state of a cell in the next configuration.
    /// @param i Linear index of the cell.
    /// @return State of the requested cell.
    /// @pre The index must be within the storage bounds.
    cell_state get_next(int i) const {
        return _next[i];
    }

    /// @brief Sets the state of a cell in the next configuration.
    /// @param i Linear index of the cell.
    /// @param state State to assign.
    /// @pre The index must be within the storage bounds.
    void set_next(int i, cell_state state) {
        _next[i] = state;
    }

    /// @brief Exchanges the current and next configuration buffers.
    void swap_buffers();

    /// @brief Sets the state of a cell in the current configuration.
    /// @param i Linear index of the cell.
    /// @param state State to assign.
    /// @pre The index must be within the storage bounds.
    void set(std::size_t i, cell_state state) {
        _current[i] = state;
    }

    /// @brief Resizes the grid and resets both configuration buffers.
    /// @param width New number of columns.
    /// @param height New number of rows.
    /// @details If either dimension is less than one, the operation
    ///          is ignored.
    void resize(int width, int height);

    /// @brief Returns the total number of cells in the grid.
    /// @return Number of stored cells.
    std::size_t size() const noexcept {
        return _current.size();
    }

    /// @brief Returns the grid width.
    /// @return Number of columns.
    int width() const noexcept {
        return _width;
    }

    /// @brief Returns the grid height.
    /// @return Number of rows.
    int height() const noexcept {
        return _height;
    }

    /// @brief Provides read-only access to the current configuration.
    /// @return Constant reference to the current cell buffer.
    const std::vector<cell_state>& current() const noexcept {
        return _current;
    }

    /// @brief Provides mutable access to the next configuration.
    /// @return Reference to the next cell buffer.
    std::vector<cell_state>& next() noexcept {
        return _next;
    }

private:
    int _width;
    int _height;

    std::vector<cell_state> _current;
    std::vector<cell_state> _next;
};

} // namespace cas::automata