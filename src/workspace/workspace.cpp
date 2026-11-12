#include <workspace/workspace.h>

#include <utility>

namespace cas {

workspace::workspace(float& ui_scale)
    : _ui_scale(ui_scale) {
}

viewports::manager& workspace::viewports() noexcept {
    return _viewports;
}

const viewports::manager& workspace::viewports() const noexcept {
    return _viewports;
}

float workspace::ui_scale() const noexcept {
    return _ui_scale;
}

void workspace::set_ui_scale(float scale) noexcept {
    _ui_scale = scale;
}

std::filesystem::path& workspace::path() noexcept {
    return _path;
}

const std::filesystem::path& workspace::path() const noexcept {
    return _path;
}

bool workspace::has_location() const noexcept {
    return !_path.empty();
}

void workspace::set_location(std::filesystem::path path) {
    _path = std::move(path);
}

void workspace::clear_location() noexcept {
    _path.clear();
}

} // namespace cas