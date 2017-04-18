#include "Engine.hpp"

namespace pse
{
    Engine::Engine(size_t screenWidth, size_t screenHeight, const std::string &screenName, double refreshRate) noexcept
            : _window(sf::VideoMode(screenWidth, screenHeight), screenName, sf::Style::Close),
              _items(),
              _refreshTimer(1 / refreshRate)
#ifdef PSE_TGUI
             , _tgui(_window)
#endif
    {
    }

    void Engine::update(bool clear) noexcept
    {
        if (clear)
            displayColor(sf::Color::Black);

        refreshMovingItems();

        drawItems();

#ifdef PSE_TGUI
        _tgui.draw();
#endif

        _window.display();
    }

    void Engine::refreshMovingItems() noexcept
    {
        // If things shouldn't move yet, fuck it
        if (!_refreshTimer.isDone())
            return;

        _refreshTimer.restart();

        for (const auto &p : _items)
        {
            auto item = p.first;
            // If item isn't supposed to move, fuck it
            if (item->getDestination().x == -1)
                continue;

            // If item hasn't reached destination, move it closer depending on how long it has left to move
            if (!isSamePosition(item->getDestination(), item->getPosition()))
            {
                float coeff = (float) (1 / (item->getMoveDuration() / _refreshTimer.getDuration().count()));
                sf::Vector2f dist = item->getDestination() - item->getPreviousPosition();
                dist *= coeff;
                item->setPosition(item->getPosition() + dist);
            }
            else // Item has reached destination, say it shouldn't move anymore
            {
                item->setDelay(-1);
                item->setDestination({-1, -1});
            }
        }
    }

    void Engine::drawItems() noexcept
    {
        size_t drawn = 0;
        size_t height = 0;

        while (drawn < _items.size())
        {
            for (const auto &p : _items)
                if (p.second == height)
                {
                    p.first->draw(_window);
                    ++drawn;
                }

            ++height;
        }
    }
}