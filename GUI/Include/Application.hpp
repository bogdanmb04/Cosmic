#pragma once

#include "IMenuListener.hpp"

namespace Pacman 
{
    class Application : public IMenuListener {
    public:
        void OnPlaySelected() override;
        void OnQuitSelected() override;
        bool ShouldStartGame() const;
        bool ShouldQuit() const;
    private:
        bool startGameRequested_ = false;
        bool quitRequested_ = false;
    };
}

