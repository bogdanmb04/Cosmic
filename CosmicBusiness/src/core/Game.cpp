#include "Game.hpp"

#include <vector>
#include <algorithm>
#include <mutex>

namespace logic {

class GameImpl : public IGame {
public:
    GameImpl() : state_(GameState::NotStarted) {}

    void add_observer(IObserver* obs) override {
        if (!obs) return;
        std::lock_guard<std::mutex> lk(mutex_);
        if (std::ranges::find(observers_, obs) == observers_.end())
            observers_.push_back(obs);
    }

    void remove_observer(IObserver* obs) override {
        std::lock_guard<std::mutex> lk(mutex_);
        observers_.erase(std::remove(observers_.begin(), observers_.end(), obs), observers_.end());
    }

    void start() override { set_state(GameState::Running); }
    void pause() override { set_state(GameState::Paused); }
    void finish() override { set_state(GameState::Finished); }
    GameState state() const override { return state_; }

private:
    void set_state(GameState s) {
        {
            std::lock_guard<std::mutex> lk(mutex_);
            if (state_ == s) return;
            state_ = s;
        }
        notify(s);
    }

    void notify(GameState s) {
        std::vector<IObserver*> copy;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            copy = observers_;
        }
        for (auto* o : copy) if (o) o->on_state_changed(s);
    }

    mutable std::mutex mutex_;
    std::vector<IObserver*> observers_;
    GameState state_;
};

std::unique_ptr<IGame> create_game() {
    return std::make_unique<GameImpl>();
}

} // namespace logic

