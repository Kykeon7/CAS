#pragma once

#include <filesystem>

#include <viewports/manager.h>

namespace cas {

class workspace {
public:
    explicit workspace(float& ui_scale);

    viewports::manager& viewports() noexcept;
    const viewports::manager& viewports() const noexcept;

    float ui_scale() const noexcept;
    void set_ui_scale(float scale) noexcept;

    std::filesystem::path& path() noexcept;
    const std::filesystem::path& path() const noexcept;
    bool has_location() const noexcept;

    void set_location(std::filesystem::path path);
    void clear_location() noexcept;

private:
    viewports::manager _viewports;
    float& _ui_scale;
    std::filesystem::path _path;
};

} // namespace cas