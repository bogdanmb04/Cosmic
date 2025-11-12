#include "App.hpp"

int main(int argc, char** argv) {
    unsigned int width = 800;
    unsigned int height = 600;

    if (argc >= 3) {
        try {
            width = static_cast<unsigned int>(std::stoul(argv[1]));
            height = static_cast<unsigned int>(std::stoul(argv[2]));
        } catch (...) {
        }
    }

    Cosmic::App app(width, height);
    app.run();
    return 0;
}
