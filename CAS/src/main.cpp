#include "application.h"

#include <iostream>

int main(int argc, char* argv[]) {

    try {
        cas::Application app;
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
