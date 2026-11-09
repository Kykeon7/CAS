
#pragma once

#include <cstddef>

namespace cas::system {

std::size_t get_memory_usage_bytes();
float get_memory_usage_mb();

std::size_t get_private_usage_bytes();
float get_private_usage_mb();

} // namespace cas::system
