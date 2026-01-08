/**
 * @file GameScreen.cpp
 * @brief Implementation of the graphical rendering logic.
 * @details Handles loading textures, sprite animations, and the "Auto-tiling" algorithm for walls.
 */

#include "GameScreen.hpp"
#include "GameConfig.hpp"
#include <iostream>
#include <functional>

namespace Pacman
{

    namespace
    {
        constexpr const char *kSoundtrackRelativePath = "/audio/pacman_theme.ogg";
    }

    GameScreen::GameScreen() = default;

    /**
     * @brief Loads all necessary textures and fonts from the disk.
     * @param assetPath The directory path where images are stored (e.g., "assets").
     * @return true if critical assets (Pacman, Map) loaded successfully, false otherwise.
     */
    bool GameScreen::LoadAssets(const std::string &assetPath)
    {
        bool success = true;

        // Load Pac-Man texture (REQUIRED)
        if (!pacmanTexture_.loadFromFile(assetPath + "/Pacman16.png"))
        {
            std::cerr << "Failed to load Pacman texture\n";
            success = false;
        }
        else
        {
            std::cout << "✅ Pac-Man texture loaded successfully\n";
        }

        // Load Ghost texture
        if (!ghostTexture_.loadFromFile(assetPath + "/Ghost16.png"))
        {
            std::cerr << "Failed to load Ghost texture from: " << assetPath << "/Ghost16.png\n";
            std::cerr << "Using fallback colored ghost rendering\n";
        }
        else
        {
            std::cout << "Ghost texture loaded successfully: "
                      << ghostTexture_.getSize().x << "x" << ghostTexture_.getSize().y << " pixels\n";
        }

        // Load Pac-Man death texture (optional, but not used)
        if (!pacmanDeathTexture_.loadFromFile(assetPath + "/PacmanDeath16.png"))
        {
            std::cerr << "Warning: Failed to load PacmanDeath texture\n";
        }

        // 🗺️ Load MAP TILESET texture (the star of the show!)
        if (!mapTexture_.loadFromFile(assetPath + "/Map16.png"))
        {
            std::cerr << "⚠️  Warning: Failed to load Map texture\n";
            std::cerr << "Will use fallback colored rendering\n";
            hasMapTexture_ = false;
        }
        else
        {
            std::cout << "✅ Map tileset loaded: "
                      << mapTexture_.getSize().x << "x"
                      << mapTexture_.getSize().y << " pixels\n";
            hasMapTexture_ = true;
        }

        // Try to load HUD font (Tries Windows default, then Linux default)
        if (!hudFont_.loadFromFile("C:/Windows/Fonts/arial.ttf") &&
            !hudFont_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"))
        {
            std::cerr << "Warning: Failed to load HUD font\n";
        }

        const std::string soundtrackPath = assetPath + kSoundtrackRelativePath;
        if (soundtrack_.openFromFile(soundtrackPath))
        {
            soundtrack_.setLoop(true);
            soundtrack_.setVolume(soundtrackVolume_);
            soundtrackLoaded_ = true;
            std::cout << "🎵 Pac-Man soundtrack loaded: " << soundtrackPath << "\n";

            if (gameState_ == GameState::Running)
            {
                soundtrack_.play();
            }
        }
        else
        {
            std::cerr << "Warning: Failed to load soundtrack from " << soundtrackPath << "\n";
            std::cerr << "Add a legally obtained Pac-Man theme file (OGG/WAV/FLAC) to assets/audio/." << "\n";
        }

        pacmanSprite_.setTexture(pacmanTexture_);

        return success;
    }

    void GameScreen::SetGameEngine(std::shared_ptr<IGameEngine> gameEngine)
    {
        gameEngine_ = std::move(gameEngine);
    }

    // --- IEventListener Implementation ---

    void GameScreen::OnTileUpdated(const TileUpdate &update)
    {
        updatedTiles_.push_back(update);
    }

    void GameScreen::OnPlayerStateChanged(const PlayerState &state)
    {
        playerState_ = state;
    }

    void GameScreen::OnGameStateChanged(GameState state)
    {
        gameState_ = state;

        if (!soundtrackLoaded_)
        {
            return;
        }

        switch (state)
        {
        case GameState::Running:
            if (soundtrack_.getStatus() != sf::Music::Playing)
            {
                soundtrack_.play();
            }
            break;

        case GameState::Paused:
            if (soundtrack_.getStatus() == sf::Music::Playing)
            {
                soundtrack_.pause();
            }
            break;

        case GameState::Victory:
        case GameState::GameOver:
            if (soundtrack_.getStatus() != sf::Music::Stopped)
            {
                soundtrack_.stop();
            }
            break;
        }
    }

    void GameScreen::OnGhostsUpdated(const std::vector<GhostState> &ghosts)
    {
        ghostStates_ = ghosts;
    }

    void GameScreen::SetPlayCallback(std::function<void()> cb)
    {
        playCallback_ = std::move(cb);
    }

    /**
     * @brief Handles UI interactions (mouse clicks) during Game Over or Victory screens.
     */
    void GameScreen::HandleEvent(const sf::Event &event, sf::RenderWindow &window)
    {
        if (gameState_ != GameState::GameOver && gameState_ != GameState::Victory)
            return;

        if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            sf::Vector2i pixelPos(event.mouseButton.x, event.mouseButton.y);
            sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);
            if (playAgainButtonRect_.contains(worldPos))
            {
                if (playCallback_)
                    playCallback_();
            }
        }
    }

    /**
     * @brief Updates sprite animation frames based on elapsed time.
     * @details Cycles Pacman frames (0-5) and Ghost frames (0-1).
     */
    void GameScreen::UpdateAnimations()
    {
        animationTimer_ += 0.016f; // ~60fps assumption

        // Pac-Man animation: 6 frames, smooth cycle
        if (animationTimer_ > 0.08f)
        { // ~12.5 fps animation speed
            animationTimer_ = 0.0f;
            pacmanFrame_ = (pacmanFrame_ + 1) % 6; // 6 frames total
        }

        // Ghost animation
        if (animationTimer_ > 0.15f)
        {
            ghostFrame_ = (ghostFrame_ + 1) % GameConfig::GhostFrameCount;
        }
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // WALL SPRITE SELECTION ALGORITHM
    // ═══════════════════════════════════════════════════════════════════════════

    /**
     * @brief Calculates the correct wall sprite index based on adjacent neighbors (Bitmasking).
     * @details Used for Auto-Tiling. Checks Up, Down, Left, Right neighbors to decide
     * if the wall is a corner, a T-junction, or a straight line.
     *
     * Formula: `down + 2 * (left + 2 * (right + 2 * up))`
     * This generates a unique index between 0 and 15 for every possible wall configuration.
     *
     * @param x Grid X coordinate.
     * @param y Grid Y coordinate.
     * @return int Index of the sprite in the texture sheet (0-15).
     */
    int GameScreen::CalculateWallSpriteIndex(int x, int y) const
    {
        if (!gameEngine_)
            return 0;

        Vector2 mapSize = gameEngine_->GetMapSize();

        // Check adjacent tiles for walls
        bool up = false, down = false, left = false, right = false;

        // Up neighbor
        if (y > 0)
        {
            up = (gameEngine_->GetTileAt({x, y - 1}) == TileType::Wall);
        }

        // Down neighbor
        if (y < mapSize.Y - 1)
        {
            down = (gameEngine_->GetTileAt({x, y + 1}) == TileType::Wall);
        }

        // Left neighbor (tunnel entrance handling)
        if (x > 0)
        {
            left = (gameEngine_->GetTileAt({x - 1, y}) == TileType::Wall);
        }
        else
        {
            left = true; // Treat tunnel entrance as connected
        }

        // Right neighbor (tunnel entrance handling)
        if (x < mapSize.X - 1)
        {
            right = (gameEngine_->GetTileAt({x + 1, y}) == TileType::Wall);
        }
        else
        {
            right = true; // Treat tunnel entrance as connected
        }

        // Calculate sprite index using bitwise formula
        int index = down + 2 * (left + 2 * (right + 2 * up));

        return index;
    }

    // ═══════════════════════════════════════════════════════════════════════════
    // MAP RENDERING - SPRITE-BASED OR FALLBACK
    // ═══════════════════════════════════════════════════════════════════════════

    /**
     * @brief Draws the entire map.
     * @details Supports two modes:
     * 1. **Sprite Mode**: Uses `Map16.png` with the auto-tiling algorithm.
     * 2. **Fallback Mode**: Uses simple colored rectangles if textures failed to load.
     */
    void GameScreen::RenderMap(sf::RenderWindow &window)
    {
        if (!gameEngine_)
            return;

        Vector2 mapSize = gameEngine_->GetMapSize();

        if (hasMapTexture_)
        {
            // ═══════════════════════════════════════════════════════════
            // 🎨 SPRITE-BASED RENDERING (Map16.png loaded)
            // ═══════════════════════════════════════════════════════════
            sf::Sprite tileSprite;
            tileSprite.setTexture(mapTexture_);

            for (int y = 0; y < mapSize.Y; ++y)
            {
                for (int x = 0; x < mapSize.X; ++x)
                {
                    TileType tile = gameEngine_->GetTileAt({x, y});

                    // Set sprite position
                    tileSprite.setPosition(
                        static_cast<float>(x * TILE_SIZE),
                        static_cast<float>(y * TILE_SIZE));

                    switch (tile)
                    {
                    case TileType::Wall:
                    {
                        // 🧱 AUTO-SELECT WALL SPRITE (0-15) based on neighbors
                        int spriteIndex = CalculateWallSpriteIndex(x, y);
                        tileSprite.setTextureRect(sf::IntRect(
                            spriteIndex * TILE_SIZE, // X: 0, 16, 32, ..., 240
                            0,                       // Y: row 0 (wall row)
                            TILE_SIZE,               // Width: 16px
                            TILE_SIZE                // Height: 16px
                            ));
                        window.draw(tileSprite);
                        break;
                    }

                    case TileType::Pellet:
                    {
                        // 🔴 PELLET: row 1, column 0
                        tileSprite.setTextureRect(sf::IntRect(
                            0,         // X: column 0
                            TILE_SIZE, // Y: row 1
                            TILE_SIZE,
                            TILE_SIZE));
                        window.draw(tileSprite);
                        break;
                    }

                    case TileType::PowerPellet:
                    {
                        // ⚡ POWER PELLET: row 1, column 1
                        tileSprite.setTextureRect(sf::IntRect(
                            TILE_SIZE, // X: column 1
                            TILE_SIZE, // Y: row 1
                            TILE_SIZE,
                            TILE_SIZE));
                        window.draw(tileSprite);
                        break;
                    }

                    case TileType::GhostDoor:
                    {
                        // 🚪 GHOST DOOR: row 1, column 2
                        tileSprite.setTextureRect(sf::IntRect(
                            2 * TILE_SIZE, // X: column 2
                            TILE_SIZE,     // Y: row 1
                            TILE_SIZE,
                            TILE_SIZE));
                        window.draw(tileSprite);
                        break;
                    }

                    default:
                        // Empty/Path tiles - no rendering (black background)
                        break;
                    }
                }
            }
        }
        else
        {
            // ═══════════════════════════════════════════════════════════
            // 🎨 FALLBACK: COLORED RECTANGLE RENDERING
            // ═══════════════════════════════════════════════════════════
            // (Code omitted for brevity in comments, logic remains same)
            sf::RectangleShape tileShape({static_cast<float>(TILE_SIZE),
                                          static_cast<float>(TILE_SIZE)});
            sf::CircleShape pelletShape;

            for (int y = 0; y < mapSize.Y; ++y)
            {
                for (int x = 0; x < mapSize.X; ++x)
                {
                    TileType tile = gameEngine_->GetTileAt({x, y});

                    // Draw tile background
                    if (tile == TileType::Wall)
                    {
                        tileShape.setFillColor(sf::Color(33, 33, 222));
                    }
                    else
                    {
                        tileShape.setFillColor(sf::Color::Black);
                    }

                    tileShape.setPosition(
                        static_cast<float>(x * TILE_SIZE),
                        static_cast<float>(y * TILE_SIZE));
                    window.draw(tileShape);

                    // Draw pellets
                    if (tile == TileType::Pellet)
                    {
                        pelletShape.setRadius(2.0f);
                        pelletShape.setFillColor(sf::Color(255, 184, 174));
                        pelletShape.setOrigin(2.0f, 2.0f);
                        pelletShape.setPosition(
                            static_cast<float>(x * TILE_SIZE) + TILE_SIZE / 2.0f,
                            static_cast<float>(y * TILE_SIZE) + TILE_SIZE / 2.0f);
                        window.draw(pelletShape);
                    }
                    else if (tile == TileType::PowerPellet)
                    {
                        pelletShape.setRadius(5.0f);
                        pelletShape.setFillColor(sf::Color(255, 184, 174));
                        pelletShape.setOrigin(5.0f, 5.0f);
                        pelletShape.setPosition(
                            static_cast<float>(x * TILE_SIZE) + TILE_SIZE / 2.0f,
                            static_cast<float>(y * TILE_SIZE) + TILE_SIZE / 2.0f);
                        window.draw(pelletShape);
                    }
                    else if (tile == TileType::GhostDoor)
                    {
                        tileShape.setFillColor(sf::Color(255, 184, 222));
                        tileShape.setPosition(
                            static_cast<float>(x * TILE_SIZE),
                            static_cast<float>(y * TILE_SIZE));
                        window.draw(tileShape);
                    }
                }
            }
        }
    }

    void GameScreen::RenderPlayer(sf::RenderWindow &window)
    {
        if (!gameEngine_)
            return;

        // Map direction to sprite sheet row
        // Row 0: Right, Row 1: Up, Row 2: Left, Row 3: Down
        int row = 0;
        switch (playerState_.CurrentDirection)
        {
        case Direction::Right:
            row = 0;
            break;
        case Direction::Up:
            row = 1;
            break;
        case Direction::Left:
            row = 2;
            break;
        case Direction::Down:
            row = 3;
            break;
        default:
            row = 0;
            break;
        }

        // Use the 6-frame animation
        pacmanSprite_.setTexture(pacmanTexture_);
        pacmanSprite_.setTextureRect(sf::IntRect(
            pacmanFrame_ * TILE_SIZE, // X: which frame (0-5)
            row * TILE_SIZE,          // Y: which direction (0-3)
            TILE_SIZE,                // Width: 16px
            TILE_SIZE                 // Height: 16px
            ));

        pacmanSprite_.setPosition(
            static_cast<float>(playerState_.Position.X * TILE_SIZE),
            static_cast<float>(playerState_.Position.Y * TILE_SIZE));

        window.draw(pacmanSprite_);
    }

    /**
     * @brief Renders the ghosts, handling colors and "Frightened" mode.
     * @details If textures fail, uses geometric shapes (Circles) as fallback.
     */
    void GameScreen::RenderGhosts(sf::RenderWindow &window)
    {
        ghostSprites_.clear();

        // Use colored shapes rendering (fallback or simple style)
        for (const auto &ghost : ghostStates_)
        {
            float posX = static_cast<float>(ghost.Position.X * TILE_SIZE);
            float posY = static_cast<float>(ghost.Position.Y * TILE_SIZE);

            if (ghost.IsEaten)
            {
                // Draw only eyes when eaten
                sf::CircleShape eyeWhite(3.5f);
                eyeWhite.setFillColor(sf::Color::White);

                sf::CircleShape pupil(1.5f);
                pupil.setFillColor(sf::Color::Blue);

                // Left eye
                eyeWhite.setPosition(posX + 3, posY + 5);
                window.draw(eyeWhite);
                pupil.setPosition(posX + 5, posY + 6);
                window.draw(pupil);

                // Right eye
                eyeWhite.setPosition(posX + 10, posY + 5);
                window.draw(eyeWhite);
                pupil.setPosition(posX + 12, posY + 6);
                window.draw(pupil);
            }
            else
            {
                // Draw ghost body
                sf::Color ghostColor;
                if (ghost.IsFrightened)
                {
                    ghostColor = sf::Color(0, 0, 200); // Dark blue when frightened
                }
                else
                {
                    switch (ghost.Type)
                    {
                    case GhostType::Red:
                        ghostColor = sf::Color(255, 0, 0);
                        break; // Red (Blinky)
                    case GhostType::Pink:
                        ghostColor = sf::Color(255, 184, 222);
                        break; // Pink (Pinky)
                    case GhostType::Blue:
                        ghostColor = sf::Color(0, 255, 255);
                        break; // Cyan (Inky)
                    case GhostType::Orange:
                        ghostColor = sf::Color(255, 165, 0);
                        break; // Orange (Clyde)
                    default:
                        ghostColor = sf::Color::Red;
                        break;
                    }
                }

                // Draw body as rounded rectangle
                sf::CircleShape head(8.0f);
                head.setFillColor(ghostColor);
                head.setPosition(posX, posY);
                window.draw(head);

                sf::RectangleShape body(sf::Vector2f(16.0f, 10.0f));
                body.setFillColor(ghostColor);
                body.setPosition(posX, posY + 6);
                window.draw(body);

                // Draw eyes
                sf::CircleShape eyeWhite(3.0f);
                eyeWhite.setFillColor(sf::Color::White);

                sf::CircleShape pupil(1.5f);
                pupil.setFillColor(sf::Color::Black);

                // Left eye
                eyeWhite.setPosition(posX + 3, posY + 4);
                window.draw(eyeWhite);
                pupil.setPosition(posX + 4.5f, posY + 5.5f);
                window.draw(pupil);

                // Right eye
                eyeWhite.setPosition(posX + 10, posY + 4);
                window.draw(eyeWhite);
                pupil.setPosition(posX + 11.5f, posY + 5.5f);
                window.draw(pupil);
            }
        }
    }

    void GameScreen::RenderHud(sf::RenderWindow &window)
    {
        // Skip if font didn't load
        if (!hudFont_.getInfo().family.empty())
        {
            sf::Text scoreText;
            scoreText.setFont(hudFont_);
            scoreText.setCharacterSize(20);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setString(
                "Score: " + std::to_string(playerState_.Score) +
                "  Lives: " + std::to_string(playerState_.Lives));
            scoreText.setPosition(10.0f, 5.0f);
            window.draw(scoreText);

            // Game state overlays
            if (gameState_ == GameState::Paused)
            {
                sf::Text pauseText;
                pauseText.setFont(hudFont_);
                pauseText.setCharacterSize(32);
                pauseText.setFillColor(sf::Color::Yellow);
                pauseText.setString("PAUSED");
                // ... positioning logic ...
                sf::FloatRect bounds = pauseText.getLocalBounds();
                pauseText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                pauseText.setPosition(
                    static_cast<float>(window.getSize().x) / 2.0f,
                    static_cast<float>(window.getSize().y) / 2.0f);
                window.draw(pauseText);
            }
            else if (gameState_ == GameState::GameOver)
            {
                // Game Over text rendering
                sf::Text gameOverText;
                gameOverText.setFont(hudFont_);
                gameOverText.setCharacterSize(48);
                gameOverText.setFillColor(sf::Color::Red);
                gameOverText.setString("GAME OVER");

                sf::FloatRect bounds = gameOverText.getLocalBounds();
                gameOverText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                float centerX = static_cast<float>(window.getSize().x) / 2.0f;
                float centerY = static_cast<float>(window.getSize().y) / 2.0f;
                gameOverText.setPosition(centerX, centerY - 40.0f);
                window.draw(gameOverText);

                // Button rendering
                const float btnW = 200.0f;
                const float btnH = 48.0f;
                sf::Vector2f btnPos(centerX - btnW / 2.0f, centerY + 10.0f);

                sf::RectangleShape btnShape(sf::Vector2f(btnW, btnH));
                btnShape.setFillColor(sf::Color(60, 60, 60));
                btnShape.setOutlineColor(sf::Color::White);
                btnShape.setOutlineThickness(2.0f);
                btnShape.setPosition(btnPos);
                window.draw(btnShape);

                sf::Text playText;
                playText.setFont(hudFont_);
                playText.setCharacterSize(24);
                playText.setFillColor(sf::Color::White);
                playText.setString("Play Again");
                sf::FloatRect ptBounds = playText.getLocalBounds();
                playText.setOrigin(ptBounds.width / 2.0f, ptBounds.height / 2.0f);
                playText.setPosition(btnPos.x + btnW / 2.0f, btnPos.y + btnH / 2.0f - 4.0f);
                window.draw(playText);

                playAgainButtonRect_ = sf::FloatRect(btnPos.x, btnPos.y, btnW, btnH);
            }
            else if (gameState_ == GameState::Victory)
            {
                // Victory rendering (similar to Game Over)
                sf::Text victoryText;
                victoryText.setFont(hudFont_);
                victoryText.setCharacterSize(48);
                victoryText.setFillColor(sf::Color::Green);
                victoryText.setString("YOU WIN!");

                sf::FloatRect bounds = victoryText.getLocalBounds();
                victoryText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
                float centerX = static_cast<float>(window.getSize().x) / 2.0f;
                float centerY = static_cast<float>(window.getSize().y) / 2.0f;
                victoryText.setPosition(centerX, centerY - 40.0f);
                window.draw(victoryText);

                const float btnW = 200.0f;
                const float btnH = 48.0f;
                sf::Vector2f btnPos(centerX - btnW / 2.0f, centerY + 10.0f);

                sf::RectangleShape btnShape(sf::Vector2f(btnW, btnH));
                btnShape.setFillColor(sf::Color(60, 60, 60));
                btnShape.setOutlineColor(sf::Color::White);
                btnShape.setOutlineThickness(2.0f);
                btnShape.setPosition(btnPos);
                window.draw(btnShape);

                sf::Text playText;
                playText.setFont(hudFont_);
                playText.setCharacterSize(24);
                playText.setFillColor(sf::Color::White);
                playText.setString("Play Again");
                sf::FloatRect ptBounds = playText.getLocalBounds();
                playText.setOrigin(ptBounds.width / 2.0f, ptBounds.height / 2.0f);
                playText.setPosition(btnPos.x + btnW / 2.0f, btnPos.y + btnH / 2.0f - 4.0f);
                window.draw(playText);

                playAgainButtonRect_ = sf::FloatRect(btnPos.x, btnPos.y, btnW, btnH);
            }

            // Power-up indicator
            if (playerState_.IsPoweredUp)
            {
                sf::Text powerText;
                powerText.setFont(hudFont_);
                powerText.setCharacterSize(18);
                powerText.setFillColor(sf::Color::Cyan);
                powerText.setString("POWER UP!");
                powerText.setPosition(10.0f, 35.0f);
                window.draw(powerText);
            }
        }
    }

    void GameScreen::Render(sf::RenderWindow &window)
    {
        UpdateAnimations();

        window.clear(sf::Color::Black);

        RenderMap(window);
        RenderPlayer(window);
        RenderGhosts(window);
        RenderHud(window);

        window.display();
    }

}