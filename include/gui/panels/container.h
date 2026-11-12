
#pragma once

#include <functional>
#include <string>
#include <utility>
#include <vector>

#include <gui/panels/panel.h>

namespace cas::gui::panels {

/// @brief Groups and renders non-owning references to GUI panels.
class container {
public:
    explicit container(std::string name) :
        _name(std::move(name))
    {
    }

    container(const container&) = delete;
    container& operator=(const container&) = delete;
    container(container&&) noexcept = default;
    container& operator=(container&&) noexcept = default;
    ~container() = default;

    /// @brief Adds a panel to the container.
    /// @param panel The panel to reference.
    void add_panel(panels::panel& panel) {
        _panels.emplace_back(panel);
    }

    /// @brief Renders the container's panels.
    /// @param visible Whether the container is visible.
    void render(bool& visible);

    /// @brief Returns the container's name.
    [[nodiscard]]
    const std::string& name() const noexcept {
        return _name;
    }

private:
    std::string _name;

    std::vector<std::reference_wrapper<panels::panel>> _panels;
};

} // namespace cas::gui::panels
