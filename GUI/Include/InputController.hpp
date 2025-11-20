#pragma once

#include <SFML/Window.hpp>
#include "IGameEngine.hpp"
#include <memory>

namespace Pacman {

    // Handles user input and translates to game commands
    class InputController {
    public:
        explicit InputController(std::shared_ptr<IGameEngine> gameEngine);

        void ProcessEvent(const sf::Event& event);

    private:
        std::shared_ptr<IGameEngine> gameEngine_;
    };

}
