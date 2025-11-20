#pragma once

#include <SFML/Graphics.hpp>
#include "IEventListener.hpp"
#include "IGameEngine.hpp"
#include <vector>
#include <memory>

namespace Pacman {

    /// @brief SFML-based renderer implementing IEventListener
    class SfmlRenderer : public IEventListener {
    public:
        SfmlRenderer();
        
        /// @brief Load rendering assets
        /// @param assetPath Path to assets directory
        /// @return True if all assets loaded successfully
        bool LoadAssets(const std::string& assetPath);
        
        /// @brief Set the game engine reference
        /// @param gameEngine Game engine to render
        void SetGameEngine(std::shared_ptr<IGameEngine> gameEngine);
        
        // IEventListener implementation
        void OnTileUpdated(const TileUpdate& update) override;
        void OnPlayerStateChanged(const PlayerState& state) override;
        void OnGameStateChanged(GameState state) override;
        void OnGhostsUpdated(const std::vector<GhostState>& ghosts) override;
        
        /// @brief Render the game
        /// @param window The window to render to
        void Render(sf::RenderWindow& window);

    private:
        void UpdateAnimations();
        void RenderMap(sf::RenderWindow& window);
        void RenderPlayer(sf::RenderWindow& window);
        void RenderGhosts(sf::RenderWindow& window);
        void RenderHud(sf::RenderWindow& window);

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

        static constexpr int TILE_SIZE = 16;
    };

}