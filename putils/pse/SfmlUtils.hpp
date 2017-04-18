#pragma once

#include <iostream>

class SfmlUtils
{
    // Convert 2D pos to 1D pos
public:
    static unsigned int convertPos(const sf::Vector2f &pos, const sf::Vector2f &tiles) noexcept
    {
        if (pos.x > tiles.x || pos.y > tiles.y)
            return 0;
        return (unsigned int) (pos.y * tiles.x + pos.x);
    }
};