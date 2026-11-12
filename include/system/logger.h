#pragma once

#include <filesystem>
#include <fstream>
#include <mutex>
#include <string_view>

namespace cas::system {

    /**
     * Defines the severity levels supported by the logger.
     */
    enum class log_level {
        trace,  ///< Detailed diagnostic information.
        debug,  ///< Information useful during development.
        info,   ///< General application information.
        warn,   ///< Potentially problematic conditions.
        error,  ///< Errors that affect an operation.
        fatal   ///< Critical errors that may prevent continued execution.
    };

    /**
     * @brief Provides thread-safe application logging through a singleton instance.
     *
     * Log messages are written to a file with timestamps, severity levels,
     * and categories. When debug mode is enabled, messages are also written
     * to the standard output or standard error streams.
     *
     * Logging operations are synchronized internally. Messages submitted
     * before initialization or after shutdown are silently discarded.
     */
    class logger final {
    public:
        /**
         * Returns the singleton logger instance.
         *
         * @return Reference to the application-wide logger.
         */
        static logger& instance();

        logger(const logger&) = delete;
        logger& operator=(const logger&) = delete;

        logger(logger&&) = delete;
        logger& operator=(logger&&) = delete;

        /**
         * Initializes the logger and opens its output file.
         *
         * Creates the parent directory if necessary. If the logger is
         * already initialized, the call has no effect.
         *
         * @param path Path to the log file.
         * @param debug_mode Enables additional output to the console.
         * @throws std::filesystem::filesystem_error If the log directory
         *         cannot be created.
         * @throws std::runtime_error If the log file cannot be opened.
         */
        void initialize(
            const std::filesystem::path& path = "logs/cas.log",
            bool console_output = false
        );

        /**
         * Flushes and closes the log file.
         *
         * If the logger is not initialized, the call has no effect.
         */
        void shutdown();

        /**
         * Writes a message with the specified severity level.
         *
         * Each message includes a timestamp, severity level, and category.
         * Messages with warning severity or higher are immediately flushed
         * to the log file.
         *
         * When debug mode is enabled, messages are also written to the
         * console. Warning and higher severity levels use standard error;
         * lower levels use standard output.
         *
         * @param level Severity level of the message.
         * @param category Category associated with the message.
         * @param message Message content.
         */
        void log(
            log_level level,
            std::string_view category,
            std::string_view message
        );

        /** Writes a trace-level message. */
        void trace(std::string_view category, std::string_view message);

        /** Writes a debug-level message. */
        void debug(std::string_view category, std::string_view message);

        /** Writes an informational message. */
        void info(std::string_view category, std::string_view message);

        /** Writes a warning message. */
        void warn(std::string_view category, std::string_view message);

        /** Writes an error message. */
        void error(std::string_view category, std::string_view message);

        /** Writes a fatal error message. */
        void fatal(std::string_view category, std::string_view message);


    private:
        logger() = default;
        ~logger();

    private:
        static std::string_view level_name(log_level level);

        std::mutex _mutex;
        std::ofstream _file;
        bool _initialized = false;
        bool _console_output = false;
    };

} // namespace cas::system
