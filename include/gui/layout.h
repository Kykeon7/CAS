#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <gui/panels/container.h>

namespace cas::gui {

class manager;

namespace panels {
class collection;
}

/**
 * @brief Manages the GUI dockspace, panel containers, and layout persistence.
 */
class layout {
public:
    /**
     * @brief Constructs the GUI layout manager.
     *
     * @param gui GUI manager used to manage the dockspace.
     */
    explicit layout(manager& gui);

    layout(const layout&) = delete;
    layout& operator=(const layout&) = delete;

    layout(layout&&) = delete;
    layout& operator=(layout&&) = delete;

    ~layout() = default;

    /**
     * @brief Creates a container and adds it to the layout.
     *
     * @param name Container name.
     * @return Reference to the created container.
     */
    panels::container& create_container(std::string name);

    /**
     * @brief Initializes layout persistence.
     *
     * @param ini_path Path to the ImGui layout file.
     * @param persistence_enabled Whether layout persistence is enabled.
     * @return True if a saved layout was found and loaded.
     */
    bool initialize(
        const std::filesystem::path& ini_path,
        bool persistence_enabled
    );

    /**
     * @brief Builds the default dockspace arrangement.
     */
    void build_default_layout();

    /**
     * @brief Renders the dockspace and its panel containers.
     *
     * @param panels Panel collection providing visibility states.
     */
    void render(panels::collection& panels);

    /**
     * @brief Requests rebuilding the default layout.
     */
    void reset_layout();

    /**
     * @brief Removes all containers and requests the default layout.
     */
    void clear();

    /**
     * @brief Loads the saved ImGui layout.
     * @return True if a saved layout was loaded.
     */
    bool load();

    /**
     * @brief Saves the current ImGui layout.
     * @return True if persistence is enabled and the save was requested.
     */
    bool save() const;

    /**
     * @brief Enables or disables layout persistence.
     *
     * @param enabled Whether persistence should be enabled.
     */
    void set_persistence_enabled(bool enabled) noexcept;

private:
    manager& _gui;
    std::vector<std::unique_ptr<panels::container>> _containers;

    std::filesystem::path _ini_path;
    bool _persistence_enabled = true;
    bool _force_default_layout = false;
    bool _has_saved_layout = false;
};

} // namespace cas::gui