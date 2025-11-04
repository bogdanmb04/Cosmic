#pragma once

#include <memory>

namespace logic {

    // Enums (part of API)
    enum class GameState { NotStarted = 0, Running = 1, Paused = 2, Finished = 3 };
    enum class Player { None = 0, Player1 = 1, Player2 = 2 };

    // Observer interface (part of API)
    struct IObserver {
        virtual void on_state_changed(GameState newState) = 0;
        virtual ~IObserver() = default;
    };

    // Game interface (part of API)
    struct IGame {
        virtual void add_observer(IObserver* obs) = 0;
        virtual void remove_observer(IObserver* obs) = 0;
        virtual void start() = 0;
        virtual void pause() = 0;
        virtual void finish() = 0;
        virtual GameState state() const = 0;
        virtual ~IGame() = default;
    };

    // Factory function (part of API)
    std::unique_ptr<IGame> create_game();

} // namespace logic

