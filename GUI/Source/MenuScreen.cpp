#include "MenuScreen.hpp"
#include <filesystem>

namespace fs = std::filesystem;

namespace Pacman {

    namespace {
        bool TryLoadFont(sf::Font& font) {
            // Try common system fonts
            const char* candidates[] = {
                "C:/Windows/Fonts/arial.ttf",
                "C:/Windows/Fonts/segoeui.ttf",
                "C:/Windows/Fonts/tahoma.ttf",
                "C:/Windows/Fonts/verdana.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Linux
                "/System/Library/Fonts/Helvetica.ttc",             // macOS
                "Assets/arial.ttf"                                 // Local
            };
            
            for(const char* path : candidates) {
                if(fs::exists(path) && font.loadFromFile(path)) {
                    return true;
                }
            }
            
            return false;
        }
    }

    MenuScreen::MenuScreen(std::shared_ptr<IMenuListener> listener) 
        : listener_(std::move(listener)) {}

    bool MenuScreen::LoadAssets(const std::string& /*assetPath*/) {
        if(!TryLoadFont(font_)) {
            return false;
        }
        
        // Title text
        titleText_.setFont(font_);
        titleText_.setString("PAC-MAN");
        titleText_.setCharacterSize(64);
        titleText_.setFillColor(sf::Color::Yellow);
        
        // Play button
        playText_.setFont(font_);
        playText_.setString("PLAY");
        playText_.setCharacterSize(42);
        playText_.setFillColor(sf::Color::White);
        
        // Quit button
        quitText_.setFont(font_);
        quitText_.setString("QUIT");
        quitText_.setCharacterSize(42);
        quitText_.setFillColor(sf::Color::White);
        
        return true;
    }

    void MenuScreen::UpdateSelection(int delta) {
        selectedIndex_ = (selectedIndex_ + delta + 2) % 2;
    }

    void MenuScreen::UpdateLayout(sf::RenderWindow& window) {
        const float width = static_cast<float>(window.getSize().x);
        const float height = static_cast<float>(window.getSize().y);

        // Center title near top
        sf::FloatRect titleBounds = titleText_.getLocalBounds();
        titleText_.setOrigin(
            titleBounds.left + titleBounds.width / 2.0f, 
            titleBounds.top + titleBounds.height / 2.0f
        );
        titleText_.setPosition(width * 0.5f, height * 0.25f);

        // Position play button
        sf::FloatRect playBounds = playText_.getLocalBounds();
        playText_.setOrigin(
            playBounds.left + playBounds.width / 2.0f, 
            playBounds.top + playBounds.height / 2.0f
        );
        playText_.setPosition(width * 0.5f, height * 0.55f);

        // Position quit button
        sf::FloatRect quitBounds = quitText_.getLocalBounds();
        quitText_.setOrigin(
            quitBounds.left + quitBounds.width / 2.0f, 
            quitBounds.top + quitBounds.height / 2.0f
        );
        quitText_.setPosition(width * 0.5f, height * 0.70f);

        // Update clickable rectangles
        playRect_ = playText_.getGlobalBounds();
        quitRect_ = quitText_.getGlobalBounds();
    }

    void MenuScreen::HandleEvent(const sf::Event& event) {
        if(event.type == sf::Event::KeyPressed) {
            if(event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                UpdateSelection(-1);
            }
            else if(event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                UpdateSelection(+1);
            }
            else if(event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                if(selectedIndex_ == 0 && listener_) {
                    listener_->OnPlaySelected();
                }
                else if(selectedIndex_ == 1 && listener_) {
                    listener_->OnQuitSelected();
                }
            }
            else if(event.key.code == sf::Keyboard::Escape) {
                if(listener_) {
                    listener_->OnQuitSelected();
                }
            }
        }
        else if(event.type == sf::Event::MouseButtonPressed) {
            if(event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(
                    static_cast<float>(event.mouseButton.x), 
                    static_cast<float>(event.mouseButton.y)
                );
                
                if(playRect_.contains(mousePos)) {
                    if(listener_) listener_->OnPlaySelected();
                }
                else if(quitRect_.contains(mousePos)) {
                    if(listener_) listener_->OnQuitSelected();
                }
            }
        }
        else if(event.type == sf::Event::MouseMoved) {
            // Hover highlight
            sf::Vector2f mousePos(
                static_cast<float>(event.mouseMove.x), 
                static_cast<float>(event.mouseMove.y)
            );
            
            if(playRect_.contains(mousePos)) {
                selectedIndex_ = 0;
            }
            else if(quitRect_.contains(mousePos)) {
                selectedIndex_ = 1;
            }
        }
        else if(event.type == sf::Event::Closed) {
            requestClose_ = true;
            if(listener_) {
                listener_->OnQuitSelected();
            }
        }
    }

    void MenuScreen::Render(sf::RenderWindow& window) {
        window.clear(sf::Color(10, 10, 30));

        UpdateLayout(window);

        // Update selection colors
        playText_.setFillColor(selectedIndex_ == 0 ? sf::Color::Cyan : sf::Color::White);
        quitText_.setFillColor(selectedIndex_ == 1 ? sf::Color::Cyan : sf::Color::White);

        // Draw button backgrounds
        auto drawBackground = [&](const sf::FloatRect& rect, bool isSelected) {
            sf::RectangleShape background;
            background.setPosition({rect.left - 20.0f, rect.top - 12.0f});
            background.setSize({rect.width + 40.0f, rect.height + 24.0f});
            background.setFillColor(isSelected ? sf::Color(40, 40, 80) : sf::Color(20, 20, 40));
            background.setOutlineThickness(3.0f);
            background.setOutlineColor(isSelected ? sf::Color::Cyan : sf::Color(60, 60, 100));
            window.draw(background);
        };

        drawBackground(playRect_, selectedIndex_ == 0);
        drawBackground(quitRect_, selectedIndex_ == 1);

        // Draw text
        window.draw(titleText_);
        window.draw(playText_);
        window.draw(quitText_);

        window.display();
    }

} // namespace Pacman
