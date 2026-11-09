
#pragma once

namespace cas::gui::panels {

class panel {
public:
    virtual void render() = 0;

    virtual ~panel() = default;
};

} // namespace cas::gui::panels
