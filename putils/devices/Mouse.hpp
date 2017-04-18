#pragma once

#include "KeyState.hpp"

using ButtonState = KeyState;

namespace putils
{
    class Mouse
    {
        // Mouse buttons
    public:
        enum Button
        {
            Left,
            Right,
            Middle,
            ButtonCount
        };

        // Constructor
    public:
        Mouse() noexcept {}

        // Add an observer to a specific key
        // F: std::function<void()>
    public:
        template<typename F>
        void addObserver(Button button, F &&observer) noexcept
        {
            _buttons[button].addObserver(std::forward<F>(observer));
        }

        // Key getters and setters
    public:
        bool isKeyPressed(Button button) const noexcept { return _buttons[button].isKeyPressed(); }

        void setKeyPressed(Button button, bool pressed) noexcept { _buttons[button].setKeyPressed(pressed); }

        // Register a click in coordinates. Pair: std::pair<size_t, size_t>
    public:
        template<typename Pair>
        void leftClick(Pair &&p) noexcept
        {
            _coord = std::forward<Pair>(p);
            _buttons[Left].setKeyPressed(true);
            leftButton();
        }

        template<typename Pair>
        void rightClick(Pair &&p) noexcept
        {
            _coord = std::forward<Pair>(p);
            _buttons[Right].setKeyPressed(true);
            rightButton();
        }

    public:
        putils::Observable<> leftButton;
        putils::Observable<> rightButton;

        // Get last click coordinates
    public:
        const std::pair<int, int> &getCoordinates() const noexcept { return _coord; }

    public:
        template<typename Pair>
        void setScreenSize(Pair &&screenSize) noexcept { _screenSize = screenSize; }

        const std::pair<int, int> &getScreenSize() const noexcept { return _screenSize; }

        // Attributes
    private:
        std::vector<ButtonState> _buttons{ButtonCount};
        std::pair<int, int> _coord{0, 0};
        std::pair<int, int> _screenSize;
    };
}
