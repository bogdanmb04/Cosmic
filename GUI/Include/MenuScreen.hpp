#pragma once

#include <SFML/Graphics.hpp>
#include "IMenuListener.hpp"
#include <memory>
#include <string>

namespace Pacman {

/**
 * @brief Enhanced main menu screen with improved visuals
 *
 * Features:
 * - Animated background with pulsing pellets
 * - Gradient background
 * - Glowing button effects
 * - Smooth hover animations
 * - Decorative border
 */
class MenuScreen {
public:
    /**
     * @brief Construct a new MenuScreen
     * @param listener Menu event listener
     */
    explicit MenuScreen(std::shared_ptr<IMenuListener> listener);

    /**
     * @brief Load menu assets (fonts)
     * @param assetPath Path to assets directory
     * @return True if assets loaded successfully
     */
    bool LoadAssets(const std::string& assetPath);

    /**
     * @brief Update menu animations
     * @param deltaTime Time elapsed since last frame
     */
    void Update(float deltaTime);

    /**
     * @brief Handle SFML events
     * @param event The SFML event to handle
     */
    void HandleEvent(const sf::Event& event);

    /**
     * @brief Render the menu
     * @param window The SFML render window
     */
    void Render(sf::RenderWindow& window);

    /**
     * @brief Check if window close was requested
     * @return True if close requested
     */
    bool ShouldClose() const { return requestClose_; }

private:
    /**
     * @brief Update the selected menu item
     * @param delta Change in selection (-1 or +1)
     */
    void UpdateSelection(int delta);

    /**
     * @brief Update layout based on window size
     * @param window The SFML render window
     */
    void UpdateLayout(sf::RenderWindow& window);

    /**
     * @brief Draw a styled button
     * @param window Render window
     * @param rect Button bounds
     * @param isSelected Whether button is selected
     */
    void DrawButton(sf::RenderWindow& window, const sf::FloatRect& rect, bool isSelected);

    /**
     * @brief Draw animated background pellets
     * @param window Render window
     */
    void DrawBackgroundPellets(sf::RenderWindow& window);

    /**
     * @brief Draw decorative border around screen
     * @param window Render window
     */
    void DrawDecorativeBorder(sf::RenderWindow& window);

    std::shared_ptr<IMenuListener> listener_;
    sf::Font font_;

    sf::Text titleText_;
    sf::Text playText_;
    sf::Text quitText_;
    int selectedIndex_ = 0;
    bool requestClose_ = false;
    float animationTimer_;

    sf::FloatRect playRect_{};
    sf::FloatRect quitRect_{};
};

}