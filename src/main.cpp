#include <application/application.h>

#include <exception>
#include <stdexcept>

#include <system/console.h>
#include <system/logger.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    auto& logger = cas::system::logger::instance();

    constexpr bool console_output = false;

    try {
        if (!cas::system::set_console_enabled(console_output))
            throw std::runtime_error("Failed to configure console.");

        logger.initialize("logs/cas.log", console_output);

        logger.info("Application", "Starting CAS.");

        {
            cas::application app;
            app.run();
        }

        logger.info("Application", "CAS shutdown completed.");

        logger.shutdown();
        return 0;
    }
    catch (const std::exception& e) {
        logger.error("Application", e.what());
        logger.shutdown();
        return 1;
    }
}