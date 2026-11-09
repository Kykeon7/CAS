#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string_view>

namespace cas::system {

enum class log_level {
    trace,
    debug,
    info,
    warn,
    error,
    fatal
};

class logger final {
public:
    static logger& instance();

    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;

    logger(logger&&) = delete;
    logger& operator=(logger&&) = delete;

    void initialize(
        const std::filesystem::path& path = "logs/cas.log",
        bool debug_mode = false
    );

    void shutdown();

    void log(
        log_level level,
        std::string_view category,
        std::string_view message
    );

    void trace(std::string_view category, std::string_view message);
    void debug(std::string_view category, std::string_view message);
    void info(std::string_view category, std::string_view message);
    void warn(std::string_view category, std::string_view message);
    void error(std::string_view category, std::string_view message);
    void fatal(std::string_view category, std::string_view message);

private:
    logger() = default;
    ~logger();

    static std::string_view level_name(log_level level);

    std::mutex _mutex;
    std::ofstream _file;
    bool _initialized = false;
    bool _debug_mode = false;

};

}