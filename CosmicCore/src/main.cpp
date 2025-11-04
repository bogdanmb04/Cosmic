#include <iostream>
#include "cosmic.hpp"

struct PrintObserver : public logic::IObserver {
    void on_state_changed(logic::GameState s) override {
        std::cout << "Observer: state changed to " << static_cast<int>(s) << std::endl;
    }
};

int main() {
    auto game = logic::create_game();
    PrintObserver obs;
    game->add_observer(&obs);

    std::cout << "Starting game..." << std::endl;
    game->start();

    std::cout << "Pausing game..." << std::endl;
    game->pause();

    std::cout << "Finishing game..." << std::endl;
    game->finish();

    return 0;
}

