#pragma once

namespace Pacman {
    class IMenuListener {
    public:
        virtual ~IMenuListener() = default;
        virtual void OnPlaySelected() = 0;
        virtual void OnQuitSelected() = 0;
    };
} // namespace Pacman
