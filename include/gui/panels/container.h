#pragma once

#include <functional>
#include <string>
#include <vector>

#include <gui/panels/panel.h>

namespace cas::gui::panels {

class container {
public:
    explicit container(std::string name)
        : _name(std::move(name))
    {}

    void add_panel(panels::panel& panel) {
        _panels.emplace_back(panel);
    }

    void render(bool& visible);

    const std::string& name() const noexcept {
        return _name;
    }

private:
    std::string _name;

    std::vector<std::reference_wrapper<panels::panel>>
        _panels;
};

} // namespace cas::gui::panels