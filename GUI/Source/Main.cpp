/**
 * @file Main.cpp
 * @brief Application entry point.
 * @details Manages the high-level application flow:
 * 1. Initializes the Menu Window.
 * 2. Waits for user selection (Play/Quit).
 * 3. Initializes the Game Engine and Game Window.
 * 4. Runs the Main Game Loop.
 */

#include <SFML/Graphics.hpp>
#include "IGameEngine.hpp"
#include "GameConfig.hpp"
#include "GameScreen.hpp"
#include "InputController.hpp"
#include "MenuScreen.hpp"
#include "IMenuListener.hpp"
#include "Application.hpp"
#include "GameEngine.hpp"
#include <memory>
#include <iostream>

int main() {
    using namespace Pacman;

    constexpr int TILE_SIZE = GameConfig::TileSize;
    constexpr int MENU_WIDTH = 56 * TILE_SIZE;
    constexpr int MENU_HEIGHT = 48 * TILE_SIZE;

    // --- PHASE 1: MENU SCREEN ---

    sf::RenderWindow menuWindow(
        sf::VideoMode(MENU_WIDTH, MENU_HEIGHT),
        "Pac-Man - Menu",
        sf::Style::Titlebar | sf::Style::Close
    );
    menuWindow.setFramerateLimit(60);

    // Create the Application state handler (Observer for Menu)
    auto application = std::make_shared<Application>();
    MenuScreen menu(application);

    if (!menu.LoadAssets("assets")) {
        std::cerr << "Warning: Failed to load menu assets\n";
    }

    sf::Clock menuClock;

    // Menu loop with animation
    while (menuWindow.isOpen() && !application->ShouldStartGame() && !application->ShouldQuit()) {
        float deltaTime = menuClock.restart().asSeconds();

        sf::Event event;
        while (menuWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                application->OnQuitSelected();
                menuWindow.close();
                break;
            }
            menu.HandleEvent(event);
        }

        if (!menuWindow.isOpen()) break;

        menu.Update(deltaTime);
        menu.Render(menuWindow);
    }

    if (application->ShouldQuit()) {
        return 0;
    }

    if (menuWindow.isOpen()) {
        menuWindow.close();
    }

    // --- PHASE 2: GAMEPLAY SCREEN ---

    // Create game engine (Backend Logic)
    auto gameEngine = GameEngine::CreateGameEngine();
    Vector2 mapSize = gameEngine->GetMapSize();

    // Create game window based on map size
    sf::RenderWindow gameWindow(
        sf::VideoMode(mapSize.X * TILE_SIZE, mapSize.Y * TILE_SIZE),
        "Pac-Man",
        sf::Style::Titlebar | sf::Style::Close
    );
    gameWindow.setFramerateLimit(60);

    // Setup renderer (Frontend Visualization)
    auto renderer = std::make_shared<GameScreen>();
    renderer->SetGameEngine(gameEngine);
    gameEngine->AddListener(renderer); // Connect Frontend to Backend

    // Define restart behavior (Callback)
    renderer->SetPlayCallback([gameEngine]() {
        if (gameEngine) gameEngine->StartNewGame();
        });

    // Setup input controller (Keyboard -> Engine Commands)
    InputController inputController(gameEngine);

    if (!renderer->LoadAssets("assets")) {
        std::cerr << "Error: Failed to load game assets\n";
        return 1;
    }

    // Start the game logic
    gameEngine->StartNewGame();

    sf::Clock gameClock;

    // Main Game loop
    while (gameWindow.isOpen()) {
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                gameWindow.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    gameWindow.close();
                }
            }

            // Delegate input processing
            inputController.ProcessEvent(event);
            renderer->HandleEvent(event, gameWindow);
        }

        float deltaTime = gameClock.restart().asSeconds();
        gameEngine->Update(deltaTime);

        renderer->Render(gameWindow);
    }

    return 0;
}