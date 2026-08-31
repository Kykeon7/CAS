#include "core/application.h"

#include <print>

int main(int argc, char* argv[]) {

    try {
        cas::Application app;
        app.run();
    }
    catch (const std::exception& e) {
        std::println("{}", e.what());
    }

    return 0;
}
