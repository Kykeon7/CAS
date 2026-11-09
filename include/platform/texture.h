#pragma once

#include <imgui.h>

namespace cas::platform {

class texture {
public:
    virtual ~texture() = default;

    texture(const texture&) = delete;
    texture& operator=(const texture&) = delete;

    [[nodiscard]]
    virtual int width() const noexcept = 0;

    [[nodiscard]]
    virtual int height() const noexcept = 0;

    [[nodiscard]]
    virtual ImTextureID imgui_id() const noexcept = 0;

protected:
    texture() = default;
};

} // namespace cas::platform