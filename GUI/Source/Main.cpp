#include <SFML/Graphics.hpp>
#include "IGameEngine.hpp"
#include "GameConfig.hpp"
#include "GameScreen.hpp"
#include "InputController.hpp"
#include "MenuScreen.hpp"
#include "IMenuListener.hpp"
#include "Application.hpp"
#include <memory>
#include <iostream>


int main() {
    using namespace Pacman;
    
    const int TILE_SIZE = GameConfig::TileSize;
    const int MENU_WIDTH = 28 * TILE_SIZE;
    const int MENU_HEIGHT = 20 * TILE_SIZE;
    
    // ========== Menu Phase ==========
    
    sf::RenderWindow menuWindow(
        sf::VideoMode(MENU_WIDTH, MENU_HEIGHT), 
        "Pac-Man - Menu"
    );
    menuWindow.setFramerateLimit(60);
    
    auto application = std::make_shared<Application>();
    MenuScreen menu(application);
    
    if(!menu.LoadAssets("Assets")) {
        std::cerr << "Warning: Failed to load menu assets\n";
    }
    
    // Menu loop
    while(menuWindow.isOpen() && !application->ShouldStartGame() && !application->ShouldQuit()) {
        sf::Event event;
        while(menuWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                application->OnQuitSelected();
                menuWindow.close();
                break;
            }
            menu.HandleEvent(event);
        }
        
        if(!menuWindow.isOpen()) break;
        
        menu.Render(menuWindow);
    }
    
    if(application->ShouldQuit()) {
        return 0;
    }
    
    if(menuWindow.isOpen()) {
        menuWindow.close();
    }
    
    // ========== Game Phase ==========
    
    // Create game engine
    auto gameEngine = CreateGameEngine();
    Vector2 mapSize = gameEngine->GetMapSize();
    
    // Create game window
    sf::RenderWindow gameWindow(
        sf::VideoMode(mapSize.X * TILE_SIZE, mapSize.Y * TILE_SIZE), 
        "Pac-Man"
    );
    gameWindow.setFramerateLimit(60);
    
    // Setup renderer
    auto renderer = std::make_shared<GameScreen>();
    renderer->SetGameEngine(gameEngine);
    gameEngine->AddListener(renderer);
    
    // Setup input controller
    InputController inputController(gameEngine);
    
    // Load game assets
    if(!renderer->LoadAssets("Assets")) {
        std::cerr << "Error: Failed to load game assets\n";
        return 1;
    }
    
    // Start the game
    gameEngine->StartNewGame();
    
    sf::Clock clock;
    
    // Game loop
    while(gameWindow.isOpen()) {
        sf::Event event;
        while(gameWindow.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            else if(event.type == sf::Event::KeyPressed) {
                if(event.key.code == sf::Keyboard::Escape) {
                    gameWindow.close();
                }
            }
            
            inputController.ProcessEvent(event);
        }
        
        float deltaTime = clock.restart().asSeconds();
        gameEngine->Update(deltaTime);
        
        renderer->Render(gameWindow);
    }
    
    return 0;
}