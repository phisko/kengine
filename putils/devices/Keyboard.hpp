#pragma once

#include <vector>
#include "KeyState.hpp"

namespace putils
{
    class Keyboard
    {
        // Mapped keys, feel free to add more
    public:
        enum Key
        {
            Escape = 0,
            Space,
            Up,
            Down,
            Left,
            Right,
            A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            KeyCount
        };

        // Constructor
    public:
        Keyboard() noexcept {}

        // Add an observer to a specific key
        // F: std::function<void()>
    public:
        template<typename F>
        void addObserver(Key key, F &&observer) noexcept { _keys[key].addObserver(std::forward<F>(observer)); }

        // Key getters and setters
    public:
        bool isKeyPressed(Key key) const noexcept { return _keys[key].isKeyPressed(); }

        void setKeyPressed(Key key, bool pressed) noexcept { _keys[key].setKeyPressed(pressed); }

        // Attributes
    private:
        std::vector<KeyState> _keys{KeyCount};
    };
}
