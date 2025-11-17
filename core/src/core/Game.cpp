#include "Game.hpp"
#include <mutex>

namespace logic {

class Game : public IGame {
public:
    Game() : state_(GameState::NotStarted) {}

    void start() override { }
    void pause() override { }
    void finish() override { }
    GameState state() const override { return state_; }

private:
    mutable std::mutex mutex_;
    GameState state_;
};

std::unique_ptr<IGame> create_game() {
    return std::make_unique<Game>();
}

} // namespace logic

