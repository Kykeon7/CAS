#pragma once

#include <gui/panels/panel.h>
#include <files/dialog_service.h>

namespace cas::gui::panels {

class file final : public panel {
public:
    explicit file(files::dialog_service& dialog_service);

    void render() override;

private:
    files::dialog_service& _dialog_service;
};

} // namespace cas::gui::panels