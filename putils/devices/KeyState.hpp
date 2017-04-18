#pragma once

#include "putils/Observable.hpp"

// Observable key which simply stores its state
// (and its value, for observers to know which key was pressed)
class KeyState : public putils::Observable<>
{
    // Constructor
public:
    KeyState() noexcept = default;

    // Pressed getters and setters
public:
    bool isKeyPressed() const noexcept { return _pressed; }

    void setKeyPressed(bool pressed) noexcept
    {
        if (_pressed == pressed)
            return;
        _pressed = pressed;
        changed();
    }

    // Attributes
private:
    bool _pressed{false};
};


