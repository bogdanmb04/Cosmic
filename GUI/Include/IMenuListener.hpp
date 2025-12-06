/**
 * @file IMenuListener.hpp
 * @brief Interface for handling main menu interactions.
 * @details Implements the Observer pattern for the MenuScreen, allowing the menu to notify the main application when buttons are pressed.
 */

#pragma once

namespace Pacman {

    /**
     * @interface IMenuListener
     * @brief Interface for classes that want to react to Main Menu events.
     * @details Usually implemented by the main Application class to handle scene transitions (e.g., from Menu to Game).
     */
    class IMenuListener {
    public:
        virtual ~IMenuListener() = default;

        /**
         * @brief Called when the "PLAY" button is selected and pressed.
         * @details The application should transition to the gameplay screen and initialize the game engine.
         */
        virtual void OnPlaySelected() = 0;

        /**
         * @brief Called when the "QUIT" button is selected and pressed.
         * @details The application should close the window and terminate the process.
         */
        virtual void OnQuitSelected() = 0;
    };
}