#include "App.hpp"

namespace Cosmic {
    App::App(unsigned int width, unsigned int height, const std::string& title)
        : width(width), height(height), title(title),
          window(sf::VideoMode({width, height}), title, sf::Style::Close | sf::Style::Resize),
          game(logic::create_game()) {
        window.setFramerateLimit(60);
    }

    void App::run() {
        while (window.isOpen()) {
            processEvents();

            sf::Time dt = clock.restart();
            update(dt);
            render();
        }
    }

    void App::processEvents() {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                continue;
            }

            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }
    }

    void App::update(const sf::Time& dt) {
        return;
    }

    void App::render() {
        window.clear(sf::Color(20, 20, 30));

        sf::CircleShape cursor(5.f);
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        cursor.setPosition({static_cast<float>(mousePos.x) - 5.f, static_cast<float>(mousePos.y) - 5.f});
        cursor.setFillColor(sf::Color::Cyan);

        window.draw(cursor);
        window.display();
    }
}

