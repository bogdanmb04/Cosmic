#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "IEventListener.hpp"
#include "IGameEngine.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace Pacman
{

    /// @brief SFML-based renderer implementing IEventListener
    /// Supports sprite-based map rendering with automatic wall tile selection
    class GameScreen : public IEventListener
    {
    public:
        GameScreen();

        /// @brief Load rendering assets
        /// @param assetPath Path to assets directory
        /// @return True if all assets loaded successfully
        bool LoadAssets(const std::string &assetPath);

        /// @brief Set the game engine reference
        /// @param gameEngine Game engine to render
        void SetGameEngine(std::shared_ptr<IGameEngine> gameEngine);

        // IEventListener implementation
        void OnTileUpdated(const TileUpdate &update) override;
        void OnPlayerStateChanged(const PlayerState &state) override;
        void OnGameStateChanged(GameState state) override;
        void OnGhostsUpdated(const std::vector<GhostState> &ghosts) override;

        /// @brief Render the game
        /// @param window The window to render to
        void Render(sf::RenderWindow &window);

        /// @brief Set a callback invoked when the Play/Play Again button is pressed
        void SetPlayCallback(std::function<void()> cb);

        /// @brief Handle an SFML event (mouse clicks for Play Again button)
        void HandleEvent(const sf::Event &event, sf::RenderWindow &window);

    private:
        void UpdateAnimations();
        void RenderMap(sf::RenderWindow &window);
        void RenderPlayer(sf::RenderWindow &window);
        void RenderGhosts(sf::RenderWindow &window);
        void RenderHud(sf::RenderWindow &window);

        /// @brief Calculate wall sprite index based on adjacent tiles (0-15)
        /// @param x X coordinate in tile space
        /// @param y Y coordinate in tile space
        /// @return Sprite index using formula: down + 2*(left + 2*(right + 2*up))
        int CalculateWallSpriteIndex(int x, int y) const;

        std::shared_ptr<IGameEngine> gameEngine_;

        // Cached state
        std::vector<TileUpdate> updatedTiles_;
        PlayerState playerState_{};
        std::vector<GhostState> ghostStates_;
        GameState gameState_ = GameState::Paused;

        // Assets
        sf::Texture pacmanTexture_;
        sf::Texture pacmanDeathTexture_;
        sf::Texture ghostTexture_;
        sf::Texture mapTexture_;
        sf::Font hudFont_;

        // Sprites
        sf::Sprite pacmanSprite_;
        std::vector<sf::Sprite> ghostSprites_;

        // Animation state
        float animationTimer_ = 0.0f;
        int pacmanFrame_ = 0;
        int ghostFrame_ = 0;

        // Asset availability flags
        bool hasMapTexture_ = false;

        static constexpr int TILE_SIZE = 16;

        std::function<void()> playCallback_;
        sf::FloatRect playAgainButtonRect_;

        // Audio
        sf::Music soundtrack_;
        bool soundtrackLoaded_ = false;
        float soundtrackVolume_ = 35.0f;
        sf::SoundBuffer gameOverBuffer_;
        sf::Sound gameOverSound_;
        bool gameOverSoundLoaded_ = false;
        float gameOverVolume_ = 65.0f;
    };

}