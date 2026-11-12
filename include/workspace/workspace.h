#pragma once

#include <filesystem>

#include <viewports/manager.h>

namespace cas {

/**
 * @brief Stores the state and location of the current workspace.
 *
 * Manages the workspace's viewports, provides access to the shared UI scale,
 * and tracks the filesystem location associated with the workspace.
 */
class workspace {
public:
    /**
     * @brief Constructs a workspace using an external UI scale value.
     *
     * @param ui_scale Reference to the UI scale value shared with the application.
     *
     * @pre The referenced UI scale variable must outlive this workspace.
     */
    explicit workspace(float& ui_scale);

    /**
     * @brief Provides mutable access to the workspace's viewport manager.
     *
     * @return Reference to the viewport manager.
     */
    viewports::manager& viewports() noexcept;

    /**
     * @brief Provides read-only access to the workspace's viewport manager.
     *
     * @return Constant reference to the viewport manager.
     */
    const viewports::manager& viewports() const noexcept;

    /**
     * @brief Returns the current UI scale.
     *
     * @return The shared UI scale value.
     */
    float ui_scale() const noexcept;

    /**
     * @brief Updates the shared UI scale.
     *
     * @param scale New UI scale value.
     */
    void set_ui_scale(float scale) noexcept;

    /**
     * @brief Provides mutable access to the workspace file path.
     *
     * @return Reference to the stored filesystem path.
     */
    std::filesystem::path& path() noexcept;

    /**
     * @brief Provides read-only access to the workspace file path.
     *
     * @return Constant reference to the stored filesystem path.
     */
    const std::filesystem::path& path() const noexcept;

    /**
     * @brief Checks whether the workspace has an associated filesystem location.
     *
     * @return True if the stored path is not empty.
     */
    bool has_location() const noexcept;

    /**
     * @brief Sets the filesystem location associated with the workspace.
     *
     * @param path Filesystem path to associate with the workspace.
     */
    void set_location(std::filesystem::path path);

    /**
     * @brief Clears the workspace's associated filesystem location.
     */
    void clear_location() noexcept;

private:
    viewports::manager _viewports;
    float& _ui_scale;
    std::filesystem::path _path;
};

} // namespace cas
