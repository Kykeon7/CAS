#include <viewports/manager.h>

#include <cassert>
#include <stdexcept>

#include <utility>

#include <string>

namespace cas::viewports {

manager::manager() {
    _viewports[0].emplace();
}

manager::viewport_id manager::create() {
    for (viewport_id id = 0; id < max_viewports; ++id) {
        if (_viewports[id])
            continue;

        const std::string name = "Viewport " + std::to_string(id + 1);

        _viewports[id].emplace();

        auto& viewport = *_viewports[id];

        viewport.set_name(name);

        if (_renderer_factory) {
            viewport.set_renderer(
                _renderer_factory(viewport)
            );
        }

        return id;
    }

    throw std::runtime_error(
        "Maximum number of viewports reached"
    );
}

void manager::destroy(viewport_id id) {
    if (!exists(id))
        return;

    if (count() == 1)
        return;

    _viewports[id].reset();

    if (_active_id != id)
        return;

    for (viewport_id next_id = 0;
        next_id < max_viewports;
        ++next_id) {

        if (!_viewports[next_id])
            continue;

        _active_change = active_viewport_change{
            .previous = id,
            .current = next_id
        };

        _active_id = next_id;

        return;
    }
}

viewport& manager::get(viewport_id id) {
    assert(exists(id));

    return *_viewports[id];
}

const viewport& manager::get(viewport_id id) const {
    assert(exists(id));

    return *_viewports[id];
}

viewport& manager::active_viewport() {
    return get(_active_id);
}

const viewport& manager::active_viewport() const {
    return get(_active_id);
}

manager::viewport_id manager::active_id() const {
    return _active_id;
}

void manager::set_active(viewport_id id) {
    assert(exists(id));

    if (_active_id == id)
        return;

    _active_change = active_viewport_change{
        .previous = _active_id,
        .current = id
    };

    _active_id = id;
}

bool manager::exists(viewport_id id) const {
    return id < max_viewports && _viewports[id].has_value();
}

size_t manager::count() const {
    size_t result = 0;

    for (const auto& viewport : _viewports) {
        if (viewport)
            ++result;
    }

    return result;
}

std::array<
    std::optional<viewport>,
    manager::max_viewports
>& manager::viewports() {
    return _viewports;
}

std::optional<manager::active_viewport_change>
    manager::consume_active_change() 
noexcept 
{
    return std::exchange(_active_change, std::nullopt);
}

void manager::set_renderer_factory(
    renderer_factory factory
) {
    _renderer_factory = std::move(factory);

    if (!_renderer_factory)
        return;

    for (auto& viewport : _viewports) {
        if (!viewport)
            continue;

        viewport->set_renderer(
            _renderer_factory(*viewport)
        );
    }
}

} // namespace cas::viewports