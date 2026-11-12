#pragma once

#include <cstddef>

namespace cas::system {

/**
 * Returns the process's resident memory usage in bytes.
 *
 * Reports the resident set size (RSS), representing the amount of
 * physical memory currently occupied by the process.
 *
 * @return Resident memory usage in bytes, or zero if the measurement
 *         cannot be retrieved.
 */
[[nodiscard]]
std::size_t get_memory_usage_bytes();

/**
 * Returns the process's resident memory usage in megabytes.
 *
 * @return Resident memory usage in MiB, or zero if the measurement
 *         cannot be retrieved.
 */
[[nodiscard]]
float get_memory_usage_mb();

/**
 * Returns the process's private memory usage in bytes.
 *
 * Reports the amount of private memory attributed to the process.
 * The exact measurement depends on the operating system.
 *
 * @return Private memory usage in bytes, or zero if the measurement
 *         cannot be retrieved.
 */
[[nodiscard]]
std::size_t get_private_usage_bytes();

/**
 * Returns the process's private memory usage in megabytes.
 *
 * @return Private memory usage in MiB, or zero if the measurement
 *         cannot be retrieved.
 */
[[nodiscard]]
float get_private_usage_mb();

} // namespace cas::system
