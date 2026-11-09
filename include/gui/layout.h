#pragma once

#include <gui/panels/container.h>

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace cas::gui {

class manager;

namespace panels {
class collection;
}

class layout {
public:
    explicit layout(manager& gui);

    layout(const layout&) = delete;
    layout& operator=(const layout&) = delete;

    panels::container& create_container(std::string name);

    bool initialize(
        const std::filesystem::path& ini_path,
        bool persistence_enabled
    );

    void build_default_layout();
    void render(gui::panels::collection& panels);
    void reset_layout();
    void clear();

    bool load();
    bool save() const;

    void set_persistence_enabled(bool enabled) noexcept;

private:
    manager& _gui;
    std::vector<std::unique_ptr<panels::container>> _containers;

    std::filesystem::path _ini_path;
    bool _persistence_enabled = true;
    bool _force_default_layout = false;
    bool _has_saved_layout = false;
};

} // namespace cas
