#pragma once

#include <concepts>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>
#include <string>
#include <string_view>

#include <gui/panels/panel.h>

namespace cas::gui::panels {

class collection {
public:
    collection() = default;

    struct panel_visibility {
        std::string name;
        bool visible = true;
    };

    void register_panel(std::string name) {
        const auto it = std::ranges::find(
            _visibility,
            name,
            &panel_visibility::name
        );

        if (it == _visibility.end()) {
            _visibility.push_back({ std::move(name), true });
        }
    }

    bool& visibility(std::string_view name) {
        const auto it = std::ranges::find(
            _visibility,
            name,
            &panel_visibility::name
        );

        if (it == _visibility.end()) {
            throw std::out_of_range(
                "Requested panel visibility does not exist."
            );
        }

        return it->visible;
    }

    const std::vector<panel_visibility>& visibility_states() const noexcept {
        return _visibility;
    }

    void reset_visibility() noexcept {
        for (auto& entry : _visibility) {
            entry.visible = true;
        }
    }

    template <typename T, typename... Args>
        requires std::derived_from<T, panels::panel>
    T& create(Args&&... args) {
        auto panel = std::make_unique<T>(
            std::forward<Args>(args)...
        );

        auto& panel_ref = *panel;
        _panels.emplace_back(std::move(panel));

        return panel_ref;
    }

    template <typename T>
        requires std::derived_from<T, panels::panel>
    T& get() {
        for (auto& panel : _panels) {
            if (auto* result = dynamic_cast<T*>(panel.get()))
                return *result;
        }

        throw std::out_of_range(
            "Requested panel does not exist."
        );
    }

    void clear() {
        _panels.clear();
    }

private:
    std::vector<std::unique_ptr<panels::panel>> _panels;
    std::vector<panel_visibility> _visibility;
};

} // namespace cas::gui::panels