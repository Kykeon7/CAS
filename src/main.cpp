#include <application/settings.h>
#include "system/logger.h"

#include <exception>
#include <application/application.h>

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    auto& logger = cas::system::logger::instance();

#ifdef NDEBUG
    constexpr bool debug_mode = false;
#else
    constexpr bool debug_mode = true;
#endif

    try {
        logger.initialize("logs/cas.log", debug_mode);

        logger.info("Application", "Starting CAS.");

        {
            cas::application app(cas::rendering::backend::cpu);
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