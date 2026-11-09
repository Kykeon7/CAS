#pragma once

#include <viewports/viewport.h>

#include <array>
#include <cstddef>
#include <optional>

#include <functional>
#include <rendering/automaton/renderer.h>

namespace cas::viewports {

class manager {
public:
    using viewport_id = size_t;

    static constexpr viewport_id max_viewports = 5;

    struct active_viewport_change {
        viewport_id previous;
        viewport_id current;
    };

public:
    using renderer_factory =
        std::function<std::unique_ptr<cas::rendering::automaton::renderer>(viewport&)>;


    manager();

    viewport_id create();
    void destroy(viewport_id id);

    viewport& get(viewport_id id);
    const viewport& get(viewport_id id) const;

    viewport& active_viewport();
    const viewport& active_viewport() const;

    viewport_id active_id() const;
    void set_active(viewport_id id);

    bool exists(viewport_id id) const;
    size_t count() const;

    std::array<std::optional<viewport>, max_viewports>&
        viewports();

    std::optional<active_viewport_change>
        consume_active_change() noexcept;

    const std::array<
        std::optional<viewport>,
        max_viewports
    >& viewports() const noexcept {
        return _viewports;
    }

    void set_renderer_factory(renderer_factory factory);

private:
    std::array<std::optional<viewport>, max_viewports> _viewports;

    std::optional<active_viewport_change> _active_change;

    renderer_factory _renderer_factory;

    viewport_id _active_id = 0;
};

} // namespace cas::viewports