#pragma once

namespace cas::system {

/**
 * @brief Logs information about the system and development environment.
 *
 * Reports the operating system and its version, processor architecture,
 * compiler and its version, C++ language standard, build configuration,
 * and versions of the libraries used by CAS.
 */
void print_system_info();

} // namespace cas::system
