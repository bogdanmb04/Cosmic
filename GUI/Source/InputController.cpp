#include "InputController.hpp"

namespace Pacman {

    InputController::InputController(std::shared_ptr<IGameEngine> gameEngine) 
        : gameEngine_(std::move(gameEngine)) {}

    void InputController::ProcessEvent(const sf::Event& event) {
        if(event.type == sf::Event::KeyPressed) {
            switch(event.key.code) {
                // Arrow keys
                case sf::Keyboard::Up:
                    gameEngine_->SetPlayerDirection(Direction::Up);
                    break;
                    
                case sf::Keyboard::Down:
                    gameEngine_->SetPlayerDirection(Direction::Down);
                    break;
                    
                case sf::Keyboard::Left:
                    gameEngine_->SetPlayerDirection(Direction::Left);
                    break;
                    
                case sf::Keyboard::Right:
                    gameEngine_->SetPlayerDirection(Direction::Right);
                    break;
                
                // WASD keys
                case sf::Keyboard::W:
                    gameEngine_->SetPlayerDirection(Direction::Up);
                    break;
                    
                case sf::Keyboard::S:
                    gameEngine_->SetPlayerDirection(Direction::Down);
                    break;
                    
                case sf::Keyboard::A:
                    gameEngine_->SetPlayerDirection(Direction::Left);
                    break;
                    
                case sf::Keyboard::D:
                    gameEngine_->SetPlayerDirection(Direction::Right);
                    break;
                
                // Pause controls
                case sf::Keyboard::P:
                case sf::Keyboard::Space:
                    gameEngine_->SetPaused(gameEngine_->GetState() != GameState::Paused);
                    break;
                    
                default:
                    break;
            }
        }
    }

}
