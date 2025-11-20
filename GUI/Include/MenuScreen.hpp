#pragma once

#include <SFML/Graphics.hpp>
#include "IMenuListener.hpp"
#include <memory>
#include <string>

namespace Pacman {

/**
 * @brief Main menu screen with keyboard and mouse navigation.
 *
 * Responsible for loading assets, handling input events and rendering the menu UI.
 */
class MenuScreen {
public:
    /**
     * @brief Construct a new MenuScreen.
     * @param listener Menu event listener used to notify about menu selections.
     */
    explicit MenuScreen(std::shared_ptr<IMenuListener> listener);

    /**
     * @brief Load menu assets such as fonts.
     * @param assetPath Path to the assets directory.
     * @return True if assets loaded successfully, false otherwise.
     */
    bool LoadAssets(const std::string& assetPath);

    /**
     * @brief Handle SFML events for menu interaction.
     * @param event The SFML event to handle.
     */
    void HandleEvent(const sf::Event& event);

    /**
     * @brief Render the menu to the provided window.
     * @param window The SFML render window to draw into.
     */
    void Render(sf::RenderWindow& window);

    /**
     * @brief Check whether a close was requested from the menu.
     * @return True if the window should close, false otherwise.
     */
    bool ShouldClose() const { return requestClose_; }

private:
    /**
     * @brief Update the currently selected menu item.
     * @param delta Change in selection index (e.g., -1 or +1).
     */
    void UpdateSelection(int delta);

    /**
     * @brief Update layout metrics based on the current window size.
     * @param window The SFML render window used to compute layout.
     */
    void UpdateLayout(sf::RenderWindow& window);

    /** @brief Listener to notify about menu actions. */
    std::shared_ptr<IMenuListener> listener_;

    /** @brief Font used for menu text. */
    sf::Font font_;

    /** @brief Title text displayed at the top of the menu. */
    sf::Text titleText_;

    /** @brief Text element for the 'Play' entry. */
    sf::Text playText_;

    /** @brief Text element for the 'Quit' entry. */
    sf::Text quitText_;

    /** @brief Currently selected index (0: Play, 1: Quit). */
    int selectedIndex_ = 0;

    /** @brief Flag indicating whether a close was requested. */
    bool requestClose_ = false;

    /** @brief Bounding box for the Play button to support mouse interaction. */
    sf::FloatRect playRect_{};

    /** @brief Bounding box for the Quit button to support mouse interaction. */
    sf::FloatRect quitRect_{};
};

} // namespace Cosmic
