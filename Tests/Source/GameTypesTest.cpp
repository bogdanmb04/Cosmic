#include <gtest/gtest.h>
#include <Vector2.hpp>
#include <PlayerState.hpp>
#include <GhostState.hpp>
#include <GameState.hpp>
#include <TileType.hpp>
#include <TileUpdate.hpp>
using namespace Pacman;

TEST(DirectionTest, GetDirectionDelta_Up) {
    Vector2 delta = GetDirectionDelta(Direction::Up);
    EXPECT_EQ(delta.X, 0);
    EXPECT_EQ(delta.Y, -1);
}

TEST(DirectionTest, GetDirectionDelta_Down) {
    Vector2 delta = GetDirectionDelta(Direction::Down);
    EXPECT_EQ(delta.X, 0);
    EXPECT_EQ(delta.Y, 1);
}

TEST(DirectionTest, GetDirectionDelta_Left) {
    Vector2 delta = GetDirectionDelta(Direction::Left);
    EXPECT_EQ(delta.X, -1);
    EXPECT_EQ(delta.Y, 0);
}

TEST(DirectionTest, GetDirectionDelta_Right) {
    Vector2 delta = GetDirectionDelta(Direction::Right);
    EXPECT_EQ(delta.X, 1);
    EXPECT_EQ(delta.Y, 0);
}

TEST(DirectionTest, GetDirectionDelta_None) {
    Vector2 delta = GetDirectionDelta(Direction::None);
    EXPECT_EQ(delta.X, 0);
    EXPECT_EQ(delta.Y, 0);
}

TEST(DirectionTest, GetOppositeDirection_Up) {
    EXPECT_EQ(GetOppositeDirection(Direction::Up), Direction::Down);
}

TEST(DirectionTest, GetOppositeDirection_Down) {
    EXPECT_EQ(GetOppositeDirection(Direction::Down), Direction::Up);
}

TEST(DirectionTest, GetOppositeDirection_Left) {
    EXPECT_EQ(GetOppositeDirection(Direction::Left), Direction::Right);
}

TEST(DirectionTest, GetOppositeDirection_Right) {
    EXPECT_EQ(GetOppositeDirection(Direction::Right), Direction::Left);
}

TEST(DirectionTest, GetOppositeDirection_None) {
    EXPECT_EQ(GetOppositeDirection(Direction::None), Direction::None);
}

TEST(DirectionTest, GetOppositeDirection_IsSymmetric) {
    EXPECT_EQ(GetOppositeDirection(GetOppositeDirection(Direction::Up)), Direction::Up);
    EXPECT_EQ(GetOppositeDirection(GetOppositeDirection(Direction::Down)), Direction::Down);
    EXPECT_EQ(GetOppositeDirection(GetOppositeDirection(Direction::Left)), Direction::Left);
    EXPECT_EQ(GetOppositeDirection(GetOppositeDirection(Direction::Right)), Direction::Right);
}

class Vector2Test : public ::testing::Test {
protected:
    Vector2 v1{3, 4};
    Vector2 v2{1, 2};
    Vector2 zero{0, 0};
};

TEST_F(Vector2Test, DefaultConstruction) {
    Vector2 v;
    EXPECT_EQ(v.X, 0);
    EXPECT_EQ(v.Y, 0);
}

TEST_F(Vector2Test, ParameterizedConstruction) {
    Vector2 v{5, 7};
    EXPECT_EQ(v.X, 5);
    EXPECT_EQ(v.Y, 7);
}

TEST_F(Vector2Test, Equality_SameValues) {
    Vector2 v3{3, 4};
    EXPECT_TRUE(v1 == v3);
}

TEST_F(Vector2Test, Equality_DifferentValues) {
    EXPECT_FALSE(v1 == v2);
}

TEST_F(Vector2Test, Inequality_DifferentValues) {
    EXPECT_TRUE(v1 != v2);
}

TEST_F(Vector2Test, Inequality_SameValues) {
    Vector2 v3{3, 4};
    EXPECT_FALSE(v1 != v3);
}

TEST_F(Vector2Test, Addition) {
    Vector2 result = v1 + v2;
    EXPECT_EQ(result.X, 4);
    EXPECT_EQ(result.Y, 6);
}

TEST_F(Vector2Test, Addition_WithZero) {
    Vector2 result = v1 + zero;
    EXPECT_EQ(result.X, v1.X);
    EXPECT_EQ(result.Y, v1.Y);
}

TEST_F(Vector2Test, Subtraction) {
    Vector2 result = v1 - v2;
    EXPECT_EQ(result.X, 2);
    EXPECT_EQ(result.Y, 2);
}

TEST_F(Vector2Test, Subtraction_WithZero) {
    Vector2 result = v1 - zero;
    EXPECT_EQ(result.X, v1.X);
    EXPECT_EQ(result.Y, v1.Y);
}

TEST_F(Vector2Test, ScalarMultiplication) {
    Vector2 result = v1 * 2;
    EXPECT_EQ(result.X, 6);
    EXPECT_EQ(result.Y, 8);
}

TEST_F(Vector2Test, ScalarMultiplication_ByZero) {
    Vector2 result = v1 * 0;
    EXPECT_EQ(result.X, 0);
    EXPECT_EQ(result.Y, 0);
}

TEST_F(Vector2Test, ScalarMultiplication_ByNegative) {
    Vector2 result = v1 * -1;
    EXPECT_EQ(result.X, -3);
    EXPECT_EQ(result.Y, -4);
}

TEST_F(Vector2Test, DistanceSquared_SamePoint) {
    EXPECT_EQ(v1.DistanceSquared(v1), 0);
}

TEST_F(Vector2Test, DistanceSquared_DifferentPoints) {
    int distSq = v1.DistanceSquared(v2);
    EXPECT_EQ(distSq, 8);
}

TEST_F(Vector2Test, DistanceSquared_ToOrigin) {
    int distSq = v1.DistanceSquared(zero);
    EXPECT_EQ(distSq, 25);
}

TEST_F(Vector2Test, Distance_SamePoint) {
    EXPECT_FLOAT_EQ(v1.Distance(v1), 0.0f);
}

TEST_F(Vector2Test, Distance_DifferentPoints) {
    float dist = v1.Distance(v2);
    EXPECT_NEAR(dist, 2.828f, 0.001f);
}

TEST_F(Vector2Test, Distance_ToOrigin) {
    float dist = v1.Distance(zero);
    EXPECT_FLOAT_EQ(dist, 5.0f);
}

TEST_F(Vector2Test, Distance_IsSymmetric) {
    EXPECT_FLOAT_EQ(v1.Distance(v2), v2.Distance(v1));
}

TEST_F(Vector2Test, NegativeCoordinates) {
    Vector2 neg{-5, -10};
    EXPECT_EQ(neg.X, -5);
    EXPECT_EQ(neg.Y, -10);
}

TEST_F(Vector2Test, MixedSignCoordinates) {
    Vector2 mixed{-3, 7};
    Vector2 result = mixed + Vector2{5, -2};
    EXPECT_EQ(result.X, 2);
    EXPECT_EQ(result.Y, 5);
}

TEST(TileUpdateTest, Construction) {
    TileUpdate update;
    update.Position = Vector2{5, 10};
    update.Type = TileType::Pellet;

    EXPECT_EQ(update.Position.X, 5);
    EXPECT_EQ(update.Position.Y, 10);
    EXPECT_EQ(update.Type, TileType::Pellet);
}

TEST(PlayerStateTest, DefaultConstruction) {
    PlayerState state;
    EXPECT_EQ(state.Position.X, 0);
    EXPECT_EQ(state.Position.Y, 0);
    EXPECT_EQ(state.CurrentDirection, Direction::None);
    EXPECT_EQ(state.Score, 0);
    EXPECT_FALSE(state.IsPoweredUp);
    EXPECT_EQ(state.Lives, 3);
}

TEST(PlayerStateTest, CustomValues) {
    PlayerState state;
    state.Position = Vector2{10, 15};
    state.CurrentDirection = Direction::Right;
    state.Score = 1000;
    state.IsPoweredUp = true;
    state.Lives = 2;

    EXPECT_EQ(state.Position.X, 10);
    EXPECT_EQ(state.Position.Y, 15);
    EXPECT_EQ(state.CurrentDirection, Direction::Right);
    EXPECT_EQ(state.Score, 1000);
    EXPECT_TRUE(state.IsPoweredUp);
    EXPECT_EQ(state.Lives, 2);
}

TEST(GhostStateTest, DefaultConstruction) {
    GhostState state;
    EXPECT_EQ(state.Position.X, 0);
    EXPECT_EQ(state.Position.Y, 0);
    EXPECT_EQ(state.CurrentDirection, Direction::None);
    EXPECT_FALSE(state.IsFrightened);
    EXPECT_FALSE(state.IsEaten);
    EXPECT_EQ(state.Type, GhostType::Red);
    EXPECT_EQ(state.Mode, GhostMode::Scatter);
    EXPECT_TRUE(state.Name.empty());
}

TEST(GhostStateTest, CustomValues) {
    GhostState state;
    state.Position = Vector2{12, 14};
    state.CurrentDirection = Direction::Up;
    state.IsFrightened = true;
    state.IsEaten = false;
    state.Type = GhostType::Pink;
    state.Mode = GhostMode::Chase;
    state.Name = "Pinky";
    state.TargetTile = Vector2{20, 25};
    state.ScatterTarget = Vector2{2, -3};

    EXPECT_EQ(state.Position.X, 12);
    EXPECT_EQ(state.Position.Y, 14);
    EXPECT_EQ(state.CurrentDirection, Direction::Up);
    EXPECT_TRUE(state.IsFrightened);
    EXPECT_FALSE(state.IsEaten);
    EXPECT_EQ(state.Type, GhostType::Pink);
    EXPECT_EQ(state.Mode, GhostMode::Chase);
    EXPECT_EQ(state.Name, "Pinky");
    EXPECT_EQ(state.TargetTile.X, 20);
    EXPECT_EQ(state.TargetTile.Y, 25);
    EXPECT_EQ(state.ScatterTarget.X, 2);
    EXPECT_EQ(state.ScatterTarget.Y, -3);
}

TEST(EnumTest, GhostTypeValues) {
    EXPECT_NE(GhostType::Red, GhostType::Pink);
    EXPECT_NE(GhostType::Red, GhostType::Blue);
    EXPECT_NE(GhostType::Red, GhostType::Orange);
}

TEST(EnumTest, GhostModeValues) {
    EXPECT_NE(GhostMode::Chase, GhostMode::Scatter);
    EXPECT_NE(GhostMode::Chase, GhostMode::Frightened);
    EXPECT_NE(GhostMode::Chase, GhostMode::Eaten);
}

TEST(EnumTest, GameStateValues) {
    EXPECT_NE(GameState::Running, GameState::Paused);
    EXPECT_NE(GameState::Running, GameState::GameOver);
    EXPECT_NE(GameState::Running, GameState::Victory);
}

TEST(EnumTest, TileTypeValues) {
    EXPECT_NE(TileType::Wall, TileType::Path);
    EXPECT_NE(TileType::Wall, TileType::Pellet);
    EXPECT_NE(TileType::Wall, TileType::PowerPellet);
    EXPECT_NE(TileType::Wall, TileType::Empty);
    EXPECT_NE(TileType::Wall, TileType::GhostDoor);
}
