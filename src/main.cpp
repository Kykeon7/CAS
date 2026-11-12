#include <application/application.h>
#include <application/settings.h>

#include <exception>

#include <system/logger.h>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    auto& logger = cas::system::logger::instance();

    constexpr bool console_output = true;

    try {
        logger.initialize("logs/cas.log", console_output);

        logger.info("Application", "Starting CAS.");

        {
            cas::application app(cas::rendering::backend::gpu);
            app.run();
        }

        // La aplicación ya se destruyó y guardó su workspace.
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