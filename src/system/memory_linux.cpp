
#include <core/system/memory.h>

#include <fstream>
#include <string>

namespace cas::system {

namespace {

    std::size_t read_proc_kb(
        const char* path,
        const char* key
    ) {
        std::ifstream file(path);
        std::string line;

        while (std::getline(file, line)) {
            if (line.compare(0, std::string(key).size(), key) != 0) {
                continue;
            }

            const auto value_start = line.find_first_of("0123456789");

            if (value_start == std::string::npos) {
                return 0;
            }

            try {
                return static_cast<std::size_t>(
                    std::stoull(line.substr(value_start))
                    );
            }
            catch (...) {
                return 0;
            }
        }

        return 0;
    }

    std::size_t read_private_resident_bytes() {
        std::ifstream file("/proc/self/smaps_rollup");

        if (!file) {
            return 0;
        }

        std::string line;
        std::size_t private_kb = 0;

        while (std::getline(file, line)) {
            if (line.compare(0, 14, "Private_Clean:") == 0 ||
                line.compare(0, 14, "Private_Dirty:") == 0 ||
                line.compare(0, 16, "Private_Hugetlb:") == 0) {
                const auto value_start =
                    line.find_first_of("0123456789");

                if (value_start != std::string::npos) {
                    try {
                        private_kb += static_cast<std::size_t>(
                            std::stoull(line.substr(value_start))
                            );
                    }
                    catch (...) {
                        return 0;
                    }
                }
            }
        }

        return private_kb * 1024;
    }

} // namespace

std::size_t get_memory_usage_bytes() {
    return read_proc_kb("/proc/self/status", "VmRSS:") * 1024;
}

float get_memory_usage_mb() {
    return static_cast<float>(
        get_memory_usage_bytes() / (1024.0 * 1024.0)
        );
}

std::size_t get_private_usage_bytes() {
    return read_private_resident_bytes();
}

float get_private_usage_mb() {
    return static_cast<float>(
        get_private_usage_bytes() / (1024.0 * 1024.0)
        );
}

} // namespace cas::system
