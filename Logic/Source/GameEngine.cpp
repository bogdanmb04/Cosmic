#include "IGameEngine.hpp"
#include "GameConfig.hpp"
#include <algorithm>
#include <mutex>
#include <array>
#include <string>

namespace Pacman {

    class GameEngine : public IGameEngine {
    public:
        GameEngine() {
            InitializeMap();
            InitializePlayer();
            InitializeGhosts();
        }

        ~GameEngine() override = default;

        void StartNewGame() override {
            std::lock_guard<std::mutex> lock(mutex_);

            // Reset player
            playerState_.Position = {13, 22};
            playerState_.CurrentDirection = Direction::Left;
            playerState_.Score = 0;
            playerState_.IsPoweredUp = false;
            playerState_.Lives = GameConfig::StartingLives;
            desiredDirection_ = Direction::Left;

            // Reset timers
            playerStepTimer_ = 0.0f;
            ghostStepTimer_ = 0.0f;
            powerUpTimer_ = 0.0f;

            // Reset game state
            gameState_ = GameState::Running;

            // Reset ghosts
            InitializeGhosts();

            // Notify listeners
            NotifyGameState();
            NotifyPlayerState();
            NotifyGhostsUpdated();
        }

        void Update(float deltaTime) override {
            std::lock_guard<std::mutex> lock(mutex_);

            if(gameState_ != GameState::Running) return;

            // Update power-up timer
            if(playerState_.IsPoweredUp) {
                powerUpTimer_ += deltaTime;
                if(powerUpTimer_ >= GameConfig::PowerUpDuration) {
                    playerState_.IsPoweredUp = false;
                    powerUpTimer_ = 0.0f;

                    // Un-frighten ghosts
                    for(auto& ghost : ghostStates_) {
                        ghost.IsFrightened = false;
                    }
                    NotifyGhostsUpdated();
                }
            }

            // Update player
            playerStepTimer_ += deltaTime;
            while(playerStepTimer_ >= GameConfig::PlayerStepInterval) {
                UpdatePlayer();
                playerStepTimer_ -= GameConfig::PlayerStepInterval;
            }

            // Update ghosts
            ghostStepTimer_ += deltaTime;
            while(ghostStepTimer_ >= GameConfig::GhostStepInterval) {
                UpdateGhosts();
                ghostStepTimer_ -= GameConfig::GhostStepInterval;
            }

            CheckCollisions();

            // Check win condition
            if(GetPelletPositions().empty()) {
                gameState_ = GameState::Victory;
                NotifyGameState();
            }
        }

        void SetPaused(bool isPaused) override {
            std::lock_guard<std::mutex> lock(mutex_);
            gameState_ = isPaused ? GameState::Paused : GameState::Running;
            NotifyGameState();
        }

        void SetPlayerDirection(Direction direction) override {
            std::lock_guard<std::mutex> lock(mutex_);
            desiredDirection_ = direction;
        }

        GameState GetState() const override {
            return gameState_;
        }

        PlayerState GetPlayerState() const override {
            return playerState_;
        }

        Vector2 GetMapSize() const override {
            return {mapWidth_, mapHeight_};
        }

        TileType GetTileAt(const Vector2& position) const override {
            if(!IsInBounds(position)) return TileType::Wall;
            return tiles_[GetIndex(position)];
        }

        std::vector<Vector2> GetPelletPositions() const override {
            std::vector<Vector2> positions;
            positions.reserve(tiles_.size());

            for(int y = 0; y < mapHeight_; ++y) {
                for(int x = 0; x < mapWidth_; ++x) {
                    Vector2 pos{x, y};
                    TileType tile = GetTileAt(pos);
                    if(tile == TileType::Pellet || tile == TileType::PowerPellet) {
                        positions.push_back(pos);
                    }
                }
            }
            return positions;
        }

        std::vector<GhostState> GetGhostStates() const override {
            return ghostStates_;
        }

        void AddListener(std::shared_ptr<IEventListener> listener) override {
            std::lock_guard<std::mutex> lock(mutex_);
            listeners_.push_back(listener);
        }

        void RemoveListener(std::shared_ptr<IEventListener> listener) override {
            std::lock_guard<std::mutex> lock(mutex_);
            listeners_.erase(
                std::remove(listeners_.begin(), listeners_.end(), listener),
                listeners_.end()
            );
        }

    private:
        // ========== Initialization ==========

        void InitializeMap() {
            // Classic Pac-Man map layout
            static const std::array<std::string, 31> LEVEL = {
                "############################",
                "#............##............#",
                "#.####.#####.##.#####.####.#",
                "#o####.#####.##.#####.####o#",
                "#.####.#####.##.#####.####.#",
                "#..........................#",
                "#.####.##.########.##.####.#",
                "#.####.##.########.##.####.#",
                "#......##....##....##......#",
                "######.##### ## #####.######",
                "     #.##### ## #####.#     ",
                "     #.##          ##.#     ",
                "     #.## ###--### ##.#     ",
                "######.##  #GGGG#  ##.######",
                "      .    #GGGG#    .      ",
                "######.##  #GGGG#  ##.######",
                "     #.## ######## ##.#     ",
                "     #.##          ##.#     ",
                "     #.## ######## ##.#     ",
                "######.## ######## ##.######",
                "#............##............#",
                "#.####.#####.##.#####.####.#",
                "#o..##................##..o#",
                "###.##.##.########.##.##.###",
                "#......##....##....##......#",
                "#.##########.##.##########.#",
                "#..........................#",
                "############################",
                "############################",
                "############################",
                "############################"
            };

            mapWidth_ = static_cast<int>(LEVEL[0].size());
            mapHeight_ = static_cast<int>(LEVEL.size());
            tiles_.assign(mapWidth_ * mapHeight_, TileType::Path);

            // Parse map
            for(int y = 0; y < mapHeight_; ++y) {
                for(int x = 0; x < mapWidth_; ++x) {
                    char c = LEVEL[y][x];
                    TileType tile = TileType::Path;

                    switch(c) {
                        case '#': tile = TileType::Wall; break;
                        case '.': tile = TileType::Pellet; break;
                        case 'o': tile = TileType::PowerPellet; break;
                        case '-': tile = TileType::GhostDoor; break;
                        case 'G': tile = TileType::Path; break;
                        default: tile = TileType::Path; break;
                    }

                    tiles_[GetIndex({x, y})] = tile;
                }
            }
        }

        void InitializePlayer() {
            playerState_.Position = {13, 22};
            playerState_.CurrentDirection = Direction::Left;
            playerState_.Score = 0;
            playerState_.IsPoweredUp = false;
            playerState_.Lives = GameConfig::StartingLives;
            desiredDirection_ = Direction::Left;
            gameState_ = GameState::Paused;
        }

        void InitializeGhosts() {
            ghostStates_.clear();
            ghostStates_.push_back({{12, 14}, Direction::Left, false, GhostType::Blinky, "Blinky"});
            ghostStates_.push_back({{13, 14}, Direction::Up, false, GhostType::Pinky, "Pinky"});
            ghostStates_.push_back({{14, 14}, Direction::Right, false, GhostType::Inky, "Inky"});
            ghostStates_.push_back({{15, 14}, Direction::Down, false, GhostType::Clyde, "Clyde"});
        }

        // ========== Helper Methods ==========

        bool IsInBounds(const Vector2& pos) const {
            return pos.X >= 0 && pos.Y >= 0 && pos.X < mapWidth_ && pos.Y < mapHeight_;
        }

        int GetIndex(const Vector2& pos) const {
            return pos.Y * mapWidth_ + pos.X;
        }

        static Vector2 GetDirectionDelta(Direction dir) {
            switch(dir) {
                case Direction::Up:    return {0, -1};
                case Direction::Down:  return {0, 1};
                case Direction::Left:  return {-1, 0};
                case Direction::Right: return {1, 0};
                default:               return {0, 0};
            }
        }

        bool CanWalk(const Vector2& from, Direction dir) const {
            Vector2 delta = GetDirectionDelta(dir);
            Vector2 next{from.X + delta.X, from.Y + delta.Y};

            // Handle tunnel wrap
            if(dir == Direction::Left && from.X == 0) next.X = mapWidth_ - 1;
            if(dir == Direction::Right && from.X == mapWidth_ - 1) next.X = 0;

            if(!IsInBounds(next)) return false;

            TileType tile = GetTileAt(next);
            return tile != TileType::Wall && tile != TileType::GhostDoor;
        }

        bool TryConsumeTile(const Vector2& pos) {
            if(!IsInBounds(pos)) return false;

            int index = GetIndex(pos);
            TileType tile = tiles_[index];

            if(tile == TileType::Pellet) {
                tiles_[index] = TileType::Path;
                playerState_.Score += GameConfig::PelletScore;
                NotifyTileUpdated({pos, TileType::Path});
                return true;
            }

            if(tile == TileType::PowerPellet) {
                tiles_[index] = TileType::Path;
                playerState_.Score += GameConfig::PowerPelletScore;
                playerState_.IsPoweredUp = true;
                powerUpTimer_ = 0.0f;

                // Frighten all ghosts
                for(auto& ghost : ghostStates_) {
                    ghost.IsFrightened = true;
                }

                NotifyTileUpdated({pos, TileType::Path});
                NotifyGhostsUpdated();
                return true;
            }

            return false;
        }

        // ========== Update Methods ==========

        void UpdatePlayer() {
            // Try desired direction first
            if(desiredDirection_ != Direction::None && CanWalk(playerState_.Position, desiredDirection_)) {
                playerState_.CurrentDirection = desiredDirection_;
            }

            // Move in current direction
            if(playerState_.CurrentDirection != Direction::None &&
               CanWalk(playerState_.Position, playerState_.CurrentDirection)) {

                Vector2 delta = GetDirectionDelta(playerState_.CurrentDirection);
                Vector2 next{playerState_.Position.X + delta.X, playerState_.Position.Y + delta.Y};

                // Handle tunnel wrap
                if(playerState_.CurrentDirection == Direction::Left && playerState_.Position.X == 0) {
                    next.X = mapWidth_ - 1;
                }
                if(playerState_.CurrentDirection == Direction::Right && playerState_.Position.X == mapWidth_ - 1) {
                    next.X = 0;
                }

                playerState_.Position = next;
                TryConsumeTile(playerState_.Position);
                NotifyPlayerState();
            }
        }

        void UpdateGhosts() {
            for(auto& ghost : ghostStates_) {
                if(ghost.IsFrightened) {
                    // Random/fleeing movement when frightened
                    Direction dirs[] = {Direction::Up, Direction::Down, Direction::Left, Direction::Right};
                    for(Direction dir : dirs) {
                        if(CanWalk(ghost.Position, dir)) {
                            MoveGhost(ghost, dir);
                            break;
                        }
                    }
                } else {
                    // Chase player
                    Direction tryDirs[4];
                    tryDirs[0] = (ghost.Position.X < playerState_.Position.X) ? Direction::Right :
                                (ghost.Position.X > playerState_.Position.X) ? Direction::Left : Direction::None;
                    tryDirs[1] = (ghost.Position.Y < playerState_.Position.Y) ? Direction::Down :
                                (ghost.Position.Y > playerState_.Position.Y) ? Direction::Up : Direction::None;
                    tryDirs[2] = tryDirs[1];
                    tryDirs[3] = tryDirs[0];

                    bool moved = false;
                    for(Direction dir : tryDirs) {
                        if(dir != Direction::None && CanWalk(ghost.Position, dir)) {
                            MoveGhost(ghost, dir);
                            moved = true;
                            break;
                        }
                    }

                    // Fallback: try any direction
                    if(!moved) {
                        for(Direction dir : {Direction::Up, Direction::Left, Direction::Down, Direction::Right}) {
                            if(CanWalk(ghost.Position, dir)) {
                                MoveGhost(ghost, dir);
                                break;
                            }
                        }
                    }
                }
            }

            NotifyGhostsUpdated();
        }

        void MoveGhost(GhostState& ghost, Direction dir) {
            Vector2 delta = GetDirectionDelta(dir);
            Vector2 next{ghost.Position.X + delta.X, ghost.Position.Y + delta.Y};

            // Handle tunnel wrap
            if(dir == Direction::Left && ghost.Position.X == 0) next.X = mapWidth_ - 1;
            if(dir == Direction::Right && ghost.Position.X == mapWidth_ - 1) next.X = 0;

            ghost.Position = next;
            ghost.CurrentDirection = dir;
        }

        void CheckCollisions() {
            for(auto& ghost : ghostStates_) {
                if(ghost.Position == playerState_.Position) {
                    if(playerState_.IsPoweredUp && ghost.IsFrightened) {
                        // Eat ghost
                        playerState_.Score += GameConfig::GhostScore;
                        ghost.Position = {13, 14}; // Respawn
                        ghost.IsFrightened = false;
                        NotifyPlayerState();
                        NotifyGhostsUpdated();
                    } else if(!ghost.IsFrightened) {
                        // Lose a life
                        playerState_.Lives--;
                        if(playerState_.Lives <= 0) {
                            gameState_ = GameState::GameOver;
                            NotifyGameState();
                        } else {
                            // Reset positions
                            playerState_.Position = {13, 22};
                            InitializeGhosts();
                            NotifyPlayerState();
                            NotifyGhostsUpdated();
                        }
                    }
                }
            }
        }

        // ========== Notification Methods ==========

        void NotifyTileUpdated(const TileUpdate& update) {
            for(auto& listener : listeners_) {
                if(listener) listener->OnTileUpdated(update);
            }
        }

        void NotifyPlayerState() {
            for(auto& listener : listeners_) {
                if(listener) listener->OnPlayerStateChanged(playerState_);
            }
        }

        void NotifyGameState() {
            for(auto& listener : listeners_) {
                if(listener) listener->OnGameStateChanged(gameState_);
            }
        }

        void NotifyGhostsUpdated() {
            for(auto& listener : listeners_) {
                if(listener) listener->OnGhostsUpdated(ghostStates_);
            }
        }

        // ========== Member Variables ==========

        mutable std::mutex mutex_;
        std::vector<std::shared_ptr<IEventListener>> listeners_;

        // Map data
        int mapWidth_{};
        int mapHeight_{};
        std::vector<TileType> tiles_;

        // Game state
        PlayerState playerState_{};
        Direction desiredDirection_ = Direction::None;
        GameState gameState_ = GameState::Paused;
        std::vector<GhostState> ghostStates_;

        // Timers
        float playerStepTimer_ = 0.0f;
        float ghostStepTimer_ = 0.0f;
        float powerUpTimer_ = 0.0f;
    };

    // Factory function implementation
    std::shared_ptr<IGameEngine> CreateGameEngine() {
        return std::make_shared<GameEngine>();
    }

}
