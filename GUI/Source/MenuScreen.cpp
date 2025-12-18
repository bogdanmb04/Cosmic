/**
 * @file MenuScreen.cpp
 * @brief Implementation of the main menu logic for the Pacman game.
 * @details Includes input handling (mouse/keyboard), graphic rendering, and background animations.
 */

#include "MenuScreen.hpp"
#include <filesystem>
#include <cmath>

namespace fs = std::filesystem;

namespace Pacman {

    namespace {
        /**
         * @brief Attempts to load a font from a list of common system paths.
         *
         * This function ensures portability across Windows, Linux, and macOS by
         * searching for standard fonts (Arial, Segoe UI, etc.) if the local asset is missing.
         *
         * @param font Reference to the sf::Font object to be loaded.
         * @return true if the font was loaded successfully, false otherwise.
         */
        bool TryLoadFont(sf::Font& font) {
            const char* candidates[] = {
                "assets/arial.ttf", // Prioritize local file
                "C:/Windows/Fonts/arial.ttf",
                "C:/Windows/Fonts/segoeui.ttf",
                "C:/Windows/Fonts/tahoma.ttf",
                "C:/Windows/Fonts/verdana.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/System/Library/Fonts/Helvetica.ttc"
            };

            for (const char* path : candidates) {
                if (fs::exists(path) && font.loadFromFile(path)) {
                    return true;
                }
            }

            return false;
        }
    }

    /**
     * @brief Menu constructor.
     * @param listener Interface that will react to button presses (Play/Quit).
     */
    MenuScreen::MenuScreen(std::shared_ptr<IMenuListener> listener)
        : listener_(std::move(listener)), animationTimer_(0.0f) {
    }

    /**
     * @brief Loads necessary resources (fonts) and initializes text.
     * @return true if resources loaded correctly.
     */
    bool MenuScreen::LoadAssets(const std::string& /*assetPath*/) {
        if (!TryLoadFont(font_)) {
            return false; // Cannot run without font
        }

        // Configure Title
        titleText_.setFont(font_);
        titleText_.setString("PAC-MAN");
        titleText_.setCharacterSize(72);
        titleText_.setFillColor(sf::Color::Yellow);
        titleText_.setOutlineColor(sf::Color(200, 150, 0));
        titleText_.setOutlineThickness(3.0f);

        // Configure Play button
        playText_.setFont(font_);
        playText_.setString("PLAY");
        playText_.setCharacterSize(48);
        playText_.setFillColor(sf::Color::White);

        // Configure Quit button
        quitText_.setFont(font_);
        quitText_.setString("QUIT");
        quitText_.setCharacterSize(48);
        quitText_.setFillColor(sf::Color::White);

        return true;
    }

    /**
     * @brief Updates the internal menu state (animations).
     * @param deltaTime Time elapsed since the last frame (in seconds).
     */
    void MenuScreen::Update(float deltaTime) {
        animationTimer_ += deltaTime;
    }

    /**
     * @brief Changes the current selection (keyboard navigation).
     * @param delta +1 for down, -1 for up.
     */
    void MenuScreen::UpdateSelection(int delta) {
        // Use math formula to cycle between 0 and 1 (Play and Quit)
        // Add 2 to avoid negative numbers in modulo.
        selectedIndex_ = (selectedIndex_ + delta + 2) % 2;
    }

    /**
     * @brief Recalculates element positions on the screen.
     *
     * This function ensures the menu remains centered even if the window is resized.
     * It also applies the "pulsing" effect to the title.
     */
    void MenuScreen::UpdateLayout(sf::RenderWindow& window) {
        const float width = static_cast<float>(window.getSize().x);
        const float height = static_cast<float>(window.getSize().y);

        // --- Title Animation ---
        // Use sin() to scale the title up and down over time
        float pulseScale = 1.0f + 0.05f * std::sin(animationTimer_ * 3.0f);

        sf::FloatRect titleBounds = titleText_.getLocalBounds();
        // Set origin to the center of the text for correct scaling
        titleText_.setOrigin(
            titleBounds.left + titleBounds.width / 2.0f,
            titleBounds.top + titleBounds.height / 2.0f
        );
        titleText_.setPosition(width * 0.5f, height * 0.18f);
        titleText_.setScale(pulseScale, pulseScale);

        // --- Play Button Positioning ---
        sf::FloatRect playBounds = playText_.getLocalBounds();
        playText_.setOrigin(
            playBounds.left + playBounds.width / 2.0f,
            playBounds.top + playBounds.height / 2.0f
        );
        playText_.setPosition(width * 0.5f, height * 0.45f);

        // --- Quit Button Positioning ---
        sf::FloatRect quitBounds = quitText_.getLocalBounds();
        quitText_.setOrigin(
            quitBounds.left + quitBounds.width / 2.0f,
            quitBounds.top + quitBounds.height / 2.0f
        );
        quitText_.setPosition(width * 0.5f, height * 0.65f);

        // --- Calculate Hitboxes ---
        const float padding = 25.0f;
        playRect_ = sf::FloatRect(
            playText_.getPosition().x - playBounds.width / 2.0f - padding,
            playText_.getPosition().y - playBounds.height / 2.0f - padding,
            playBounds.width + padding * 2,
            playBounds.height + padding * 2
        );

        quitRect_ = sf::FloatRect(
            quitText_.getPosition().x - quitBounds.width / 2.0f - padding,
            quitText_.getPosition().y - quitBounds.height / 2.0f - padding,
            quitBounds.width + padding * 2,
            quitBounds.height + padding * 2
        );
    }

    /**
     * @brief Handles input events (keyboard and mouse).
     * @param event The current SFML event.
     */
    void MenuScreen::HandleEvent(const sf::Event& event) {
        // --- Keyboard Input ---
        if (event.type == sf::Event::KeyPressed) {
            // Navigate Up/Down
            if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W) {
                UpdateSelection(-1);
            }
            else if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S) {
                UpdateSelection(+1);
            }
            // Confirm (Enter/Space)
            else if (event.key.code == sf::Keyboard::Enter || event.key.code == sf::Keyboard::Space) {
                if (selectedIndex_ == 0 && listener_) {
                    listener_->OnPlaySelected();
                }
                else if (selectedIndex_ == 1 && listener_) {
                    listener_->OnQuitSelected();
                }
            }
            // Quick Exit with ESC
            else if (event.key.code == sf::Keyboard::Escape) {
                if (listener_) {
                    listener_->OnQuitSelected();
                }
            }
        }
        // --- Mouse Click Input ---
        else if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mousePos(
                    static_cast<float>(event.mouseButton.x),
                    static_cast<float>(event.mouseButton.y)
                );

                if (playRect_.contains(mousePos)) {
                    if (listener_) listener_->OnPlaySelected();
                }
                else if (quitRect_.contains(mousePos)) {
                    if (listener_) listener_->OnQuitSelected();
                }
            }
        }
        // --- Mouse Movement (Hover) ---
        else if (event.type == sf::Event::MouseMoved) {
            sf::Vector2f mousePos(
                static_cast<float>(event.mouseMove.x),
                static_cast<float>(event.mouseMove.y)
            );

            // Automatically select the button under the mouse
            if (playRect_.contains(mousePos)) {
                selectedIndex_ = 0;
            }
            else if (quitRect_.contains(mousePos)) {
                selectedIndex_ = 1;
            }
        }
        else if (event.type == sf::Event::Closed) {
            requestClose_ = true;
            if (listener_) {
                listener_->OnQuitSelected();
            }
        }
    }

    /**
     * @brief Renders the entire menu to the window.
     * @param window The SFML render window.
     */
    void MenuScreen::Render(sf::RenderWindow& window) {
        // 1. Draw background (simple gradient from two rectangles)
        sf::RectangleShape bgTop(sf::Vector2f(window.getSize().x, window.getSize().y / 2.0f));
        bgTop.setFillColor(sf::Color(10, 10, 50));
        bgTop.setPosition(0, 0);
        window.draw(bgTop);

        sf::RectangleShape bgBottom(sf::Vector2f(window.getSize().x, window.getSize().y / 2.0f));
        bgBottom.setFillColor(sf::Color(0, 0, 15));
        bgBottom.setPosition(0, window.getSize().y / 2.0f);
        window.draw(bgBottom);

        // 2. Draw background effects (animated pellets)
        DrawBackgroundPellets(window);

        // 3. Recalculate layout before drawing
        UpdateLayout(window);

        bool playSelected = (selectedIndex_ == 0);
        bool quitSelected = (selectedIndex_ == 1);

        // 4. Set text colors based on selection
        playText_.setFillColor(playSelected ? sf::Color::Cyan : sf::Color::White);
        quitText_.setFillColor(quitSelected ? sf::Color::Cyan : sf::Color::White);

        // Add subtle outline for selected text
        if (playSelected) {
            playText_.setOutlineColor(sf::Color(0, 255, 255, 100));
            playText_.setOutlineThickness(2.0f);
        }
        else {
            playText_.setOutlineThickness(0.0f);
        }

        if (quitSelected) {
            quitText_.setOutlineColor(sf::Color(0, 255, 255, 100));
            quitText_.setOutlineThickness(2.0f);
        }
        else {
            quitText_.setOutlineThickness(0.0f);
        }

        // 5. Draw buttons and their boxes
        DrawButton(window, playRect_, playSelected);
        DrawButton(window, quitRect_, quitSelected);

        DrawDecorativeBorder(window);

        // 6. Draw text on top
        window.draw(titleText_);
        window.draw(playText_);
        window.draw(quitText_);

        window.display();
    }

    /**
     * @brief Helper to draw a button with visual effects.
     * @param rect Dimensions and position of the button.
     * @param isSelected Whether the button is currently selected/hovered.
     */
    void MenuScreen::DrawButton(sf::RenderWindow& window, const sf::FloatRect& rect, bool isSelected) {
        // Glow effect if selected
        if (isSelected) {
            sf::RectangleShape glow;
            float glowSize = 8.0f;
            glow.setPosition(rect.left - glowSize, rect.top - glowSize);
            glow.setSize(sf::Vector2f(rect.width + glowSize * 2, rect.height + glowSize * 2));
            glow.setFillColor(sf::Color(0, 150, 150, 30));
            window.draw(glow);
        }

        // Button body
        sf::RectangleShape background;
        background.setPosition(rect.left, rect.top);
        background.setSize(sf::Vector2f(rect.width, rect.height));
        // Lighter color if selected
        background.setFillColor(isSelected ? sf::Color(20, 40, 60, 180) : sf::Color(15, 15, 35, 150));
        background.setOutlineThickness(3.0f);
        background.setOutlineColor(isSelected ? sf::Color::Cyan : sf::Color(60, 60, 120));
        window.draw(background);

        // Highlight (small reflection in top-left corner)
        sf::RectangleShape highlight;
        highlight.setPosition(rect.left + 3, rect.top + 3);
        highlight.setSize(sf::Vector2f(rect.width - 6, 2));
        highlight.setFillColor(sf::Color(255, 255, 255, isSelected ? 60 : 30));
        window.draw(highlight);
    }

    /**
     * @brief Draws a grid of pellets in the background with a wave animation.
     * @details Uses the sin() function based on grid position (row/col) and time to modify opacity.
     */
    void MenuScreen::DrawBackgroundPellets(sf::RenderWindow& window) {
        const int rows = 8;
        const int cols = 14;
        const float spacing = window.getSize().x / (cols + 1.0f);
        const float vSpacing = window.getSize().y / (rows + 1.0f);

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                float x = spacing * (col + 1);
                float y = vSpacing * (row + 1);

                // Wave effect animation
                float wave = std::sin(animationTimer_ * 2.0f + col * 0.3f + row * 0.2f);
                // Map sin value (-1 to 1) to alpha transparency
                float alpha = 30.0f + wave * 15.0f;

                sf::CircleShape pellet(3.0f);
                pellet.setFillColor(sf::Color(255, 184, 174, static_cast<sf::Uint8>(alpha)));
                pellet.setPosition(x - 3.0f, y - 3.0f);
                window.draw(pellet);
            }
        }
    }

    void MenuScreen::DrawDecorativeBorder(sf::RenderWindow& window) {
        const float thickness = 4.0f;
        const float margin = 20.0f;
        const float width = window.getSize().x - margin * 2;
        const float height = window.getSize().y - margin * 2;

        // Draw borders (retro arcade style)
        // Top
        sf::RectangleShape topBorder(sf::Vector2f(width, thickness));
        topBorder.setPosition(margin, margin);
        topBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(topBorder);

        // Bottom
        sf::RectangleShape bottomBorder(sf::Vector2f(width, thickness));
        bottomBorder.setPosition(margin, window.getSize().y - margin - thickness);
        bottomBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(bottomBorder);

        // Left
        sf::RectangleShape leftBorder(sf::Vector2f(thickness, height));
        leftBorder.setPosition(margin, margin);
        leftBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(leftBorder);

        // Right
        sf::RectangleShape rightBorder(sf::Vector2f(thickness, height));
        rightBorder.setPosition(window.getSize().x - margin - thickness, margin);
        rightBorder.setFillColor(sf::Color(33, 33, 222, 150));
        window.draw(rightBorder);
    }

}