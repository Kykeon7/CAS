#pragma once

namespace cas::system {

/**
 * @brief Enables or disables application console access.
 *
 * @param enabled Whether console access should be enabled.
 * @return true if the operation succeeded.
 */
[[nodiscard]] bool set_console_enabled(bool enabled);

} // namespace cas::system
