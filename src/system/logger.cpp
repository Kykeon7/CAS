#include <system/logger.h>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <system_error>

namespace cas::system {

logger& logger::instance() {
    static logger instance;
    return instance;
}

logger::~logger() {
    shutdown();
}

void logger::initialize(
    const std::filesystem::path& path,
    bool console_output
)
{
    std::lock_guard lock(_mutex);

    if (_initialized)
        return;

    const auto parent = path.parent_path();

    if (!parent.empty()) {
        std::error_code error_code;

        std::filesystem::create_directories(
            parent,
            error_code
        );  

        if (error_code) {
            throw std::filesystem::filesystem_error(
                "Failed to create log directory",
                parent,
                error_code
            );
        }
    }

    _file.open(
        path,
        std::ios::out | std::ios::app
    );

    if (!_file.is_open()) {
        throw std::runtime_error(
            "Failed to open log file: " + path.string()
        );
    }

    _console_output = console_output;
    _initialized = true;

    // Do not call log() here: initialize already owns _mutex.
    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count() % 1000;

    _file
        << '\n'
        << '['
        << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S")
        << '.'
        << std::setfill('0')
        << std::setw(3)
        << milliseconds
        << "] [INFO ] [Logger] Logging initialized.\n";

    _file.flush();
}

void logger::shutdown() {
    std::lock_guard lock(_mutex);

    if (!_initialized)
        return;

    _file.flush();
    _file.close();

    _initialized = false;
}

void logger::log(
    log_level level,
    std::string_view category,
    std::string_view message
)
{
    std::lock_guard lock(_mutex);

    // Logging before initialization or after shutdown is a no-op.
    if (!_initialized || !_file.is_open())
        return;

    const auto now = std::chrono::system_clock::now();
    const auto time = std::chrono::system_clock::to_time_t(now);

    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &time);
#else
    localtime_r(&time, &local_time);
#endif

    const auto milliseconds =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()
        ).count() % 1000;

    _file
        << '['
        << std::put_time(&local_time, "%Y-%m-%d %H:%M:%S")
        << '.'
        << std::setfill('0')
        << std::setw(3)
        << milliseconds
        << "] ["
        << std::left
        << std::setw(5)
        << level_name(level)
        << "] ["
        << category
        << "] "
        << message
        << '\n';

    if (level >= log_level::warn)
        _file.flush();

    if (!_console_output)
        return;

    auto& output = level >= log_level::warn
        ? std::cerr
        : std::cout;

    output
        << '[' << level_name(level) << "] ["
        << category << "] "
        << message << '\n';
}

void logger::trace(
    std::string_view category,
    std::string_view message
)
{
    log(log_level::trace, category, message);
}

void logger::debug(
    std::string_view category,
    std::string_view message
)
{
    log(log_level::debug, category, message);
}

void logger::info(
    std::string_view category,
    std::string_view message
)
{
    log(log_level::info, category, message);
}

void logger::warn(
    std::string_view category,
    std::string_view message
)
{
    log(log_level::warn, category, message);
}

void logger::error(
    std::string_view category,
    std::string_view message
)
{
    log(log_level::error, category, message);
}

void logger::fatal(
    std::string_view category,
    std::string_view message
)
{
    log(log_level::fatal, category, message);
}

std::string_view logger::level_name(log_level level) {
    switch (level) {
    case log_level::trace: return "TRACE";
    case log_level::debug: return "DEBUG";
    case log_level::info:  return "INFO";
    case log_level::warn:  return "WARN";
    case log_level::error: return "ERROR";
    case log_level::fatal: return "FATAL";
    }

    return "UNKNOWN";
}

} // namespace cas::system