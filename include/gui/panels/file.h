#pragma once

#include <files/dialog_service.h>

#include <gui/panels/panel.h>

namespace cas::gui::panels {

/// @brief Provides file operation controls for the application.
class file final : public panel {
public:
    explicit file(files::dialog_service& dialog_service);

    file(const file&) = delete;
    file& operator=(const file&) = delete;

    file(file&&) = delete;
    file& operator=(file&&) = delete;

    ~file() override = default;

    /// @brief Renders the file operation controls.
    void render() override;

private:
    files::dialog_service& _dialog_service;
};

} // namespace cas::gui::panels