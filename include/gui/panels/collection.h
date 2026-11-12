#pragma once

#include <concepts>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gui/panels/panel.h>

namespace cas::gui::panels {

/// @brief Owns and manages the application's GUI panels.
class collection {
public:
    collection() = default;
    collection(const collection&) = delete;
    collection& operator=(const collection&) = delete;
    collection(collection&&) noexcept = default;
    collection& operator=(collection&&) noexcept = default;
    ~collection() = default;

    /// @brief Stores the visibility state of a registered panel.
    struct panel_visibility {
        std::string name;
        bool visible = true;
    };

    /// @brief Registers a panel visibility entry if it does not exist.
    /// @param name The panel's unique name.
    void register_panel(std::string name) {
        const auto it = std::ranges::find(
            _visibility,
            name,
            &panel_visibility::name
        );

        if (it == _visibility.end())
            _visibility.push_back({ std::move(name), true });
    }

    /// @brief Returns the visibility state of a registered panel.
    /// @param name The panel's registered name.
    /// @return A reference to its visibility state.
    /// @throws std::out_of_range If the panel is not registered.
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

    /// @brief Returns the visibility states of all registered panels.
    [[nodiscard]]
    const std::vector<panel_visibility>& visibility_states() const noexcept {
        return _visibility;
    }

    /// @brief Makes all registered panels visible.
    void reset_visibility() noexcept {
        for (auto& entry : _visibility)
            entry.visible = true;
    }

    /// @brief Creates and stores a panel.
    /// @tparam T The concrete panel type.
    /// @tparam Args The constructor argument types.
    /// @param args Arguments forwarded to the panel constructor.
    /// @return A reference to the newly created panel.
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

    /// @brief Returns the first panel matching the requested type.
    /// @tparam T The requested panel type.
    /// @return A reference to the requested panel.
    /// @throws std::out_of_range If no matching panel exists.
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

    /// @brief Destroys all owned panels.
    void clear() noexcept {
        _panels.clear();
    }

private:
    std::vector<std::unique_ptr<panels::panel>> _panels;
    std::vector<panel_visibility> _visibility;
};

} // namespace cas::gui::panels
