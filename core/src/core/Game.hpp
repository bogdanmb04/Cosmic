#pragma once

#include <memory>

namespace logic {

    enum class GameState { NotStarted = 0, Running = 1, Paused = 2, Finished = 3 };

    struct IGame {
        virtual void start() = 0;
        virtual void pause() = 0;
        virtual void finish() = 0;
        virtual GameState state() const = 0;
        virtual ~IGame() = default;
    };

    std::unique_ptr<IGame> create_game();

} // namespace logic

