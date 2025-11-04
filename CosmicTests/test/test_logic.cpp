#include "Cosmic.hpp"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::_;

class MockObserver : public logic::IObserver {
public:
    MOCK_METHOD(void, on_state_changed, (logic::GameState), (override));
};

TEST(GameLogicTest, ObserverNotifiedOnStateChange) {
    auto game = logic::create_game();
    MockObserver mock;

    EXPECT_CALL(mock, on_state_changed(logic::GameState::Running)).Times(1);

    game->add_observer(&mock);
    game->start();
}

TEST(GameLogicTest, ObserverNotNotifiedAfterRemoval) {
    auto game = logic::create_game();
    MockObserver mock;

    EXPECT_CALL(mock, on_state_changed(_)).Times(0);

    game->add_observer(&mock);
    game->remove_observer(&mock);
    game->start();
}

TEST(GameLogicTest, InitialStateIsNotStarted) {
    auto game = logic::create_game();
    EXPECT_EQ(game->state(), logic::GameState::NotStarted);
}

TEST(GameLogicTest, StateTransitions) {
    auto game = logic::create_game();

    game->start();
    EXPECT_EQ(game->state(), logic::GameState::Running);

    game->pause();
    EXPECT_EQ(game->state(), logic::GameState::Paused);

    game->finish();
    EXPECT_EQ(game->state(), logic::GameState::Finished);
}

