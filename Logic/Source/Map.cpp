#include "Map.hpp"

namespace Pacman
{
    Map::Map()
    {
        Initialize();
    }

	void Map::Initialize()
	{
        static const std::array<std::string, 31> LEVEL = { {
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
            "######.##### ## #####.######",
            "######.##          ##.######",
            "######.## ###--### ##.######",
            "######.##  #GGGG#  ##.######",
            "      .    #GGGG#    .      ",
            "######.##  #GGGG#  ##.######",
            "######.## ######## ##.######",
            "######.##          ##.######",
            "######.## ######## ##.######",
            "######.## ######## ##.######",
            "#............##............#",
            "#.####.#####.##.#####.####.#",
            "#.####.#####.##.#####.####.#",
            "#o..##.......  .......##..o#",
            "###.##.##.########.##.##.###",
            "###.##.##.########.##.##.###",
            "#......##....##....##......#",
            "#.##########.##.##########.#",
            "#.##########.##.##########.#",
            "#..........................#",
            "############################"
        } };

        width_ = static_cast<int>(LEVEL[0].size());
        height_ = static_cast<int>(LEVEL.size());
        tiles_.assign(width_ * height_, TileType::Path);
        pelletCount_ = 0;

        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                char c = (x < static_cast<int>(LEVEL[y].size())) ? LEVEL[y][x] : ' ';
                TileType tile = TileType::Path;

                switch (c) {
                case '#': tile = TileType::Wall; break;
                case '.': tile = TileType::Pellet; pelletCount_++; break;
                case 'o': tile = TileType::PowerPellet; pelletCount_++; break;
                case '-': tile = TileType::GhostDoor; break;
                case 'G': tile = TileType::Path; break; // Ghost house interior
                default:  tile = TileType::Path; break;
                }
                tiles_[y * width_ + x] = tile;
            }
        }
        initialPelletCount_ = pelletCount_;
	}

    TileType Map::GetTileAt(const Vector2& pos) const
    {
        if (!IsInBounds(pos)) return TileType::Wall;
        return tiles_[pos.Y * width_ + pos.X];
    }

    void Map::SetTileAt(const Vector2& pos, TileType type)
    {
        if (!IsInBounds(pos)) return;
        TileType old = tiles_[pos.Y * width_ + pos.X];
        tiles_[pos.Y * width_ + pos.X] = type;

        if ((old == TileType::Pellet || old == TileType::PowerPellet) &&
            (type != TileType::Pellet && type != TileType::PowerPellet)) {
            pelletCount_--;
        }
    }

    bool Map::IsInBounds(const Vector2& pos) const
    {
        return pos.X >= 0 && pos.Y >= 0 && pos.X < width_ && pos.Y < height_;
    }

    bool Map::IsWalkable(const Vector2& pos) const
    {
        TileType tile = GetTileAt(pos);
        return tile != TileType::Wall && tile != TileType::GhostDoor;
    }

    bool Map::IsGhostWalkable(const Vector2& pos, bool canUseGhostDoor) const
    {
        TileType tile = GetTileAt(pos);
        if (tile == TileType::Wall) return false;
        if (tile == TileType::GhostDoor) return canUseGhostDoor;
        return true;
    }

    Vector2 Map::WrapPosition(const Vector2& pos) const
    {
        Vector2 wrapped = pos;
        if (wrapped.X < 0) wrapped.X = width_ - 1;
        else if (wrapped.X >= width_) wrapped.X = 0;
        return wrapped;
    }

    int Map::GetWidth() const
    {
        return width_;
    }

    int Map::GetHeight() const
    {
        return height_;
    }

    Vector2 Map::GetSize() const
    {
        return Vector2{ width_, height_ };
    }

    int Map::GetPelletCount() const
    {
        return pelletCount_;
    }

    int Map::GetInitialPelletCount() const
    {
        return initialPelletCount_;
    }

    std::vector<Vector2> Map::GetPelletPositions() const
    {
        std::vector<Vector2> positions;
        positions.reserve(pelletCount_);

        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                TileType tile = GetTileAt({ x, y });
                if (tile == TileType::Pellet || tile == TileType::PowerPellet) {
                    positions.push_back({ x, y });
                }
            }
        }
        return positions;
    }
}
