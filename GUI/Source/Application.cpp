#include "Application.hpp"
namespace Pacman {
    void Application::OnPlaySelected() {
        startGameRequested_ = true;
    }

    void Application::OnQuitSelected() {
        quitRequested_ = true;
    }

    bool Application::ShouldStartGame() const {
        return startGameRequested_;
    }

    bool Application::ShouldQuit() const {
        return quitRequested_;
    }
}

