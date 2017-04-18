#include "Grid.hpp"

namespace pse
{
    Grid::Grid(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Vector2f &tiles) noexcept
            :
            _pos(pos),
            _size(size),
            _tiles(tiles),
            _tileSize(size.x / tiles.x, size.y / tiles.y),
            _items(SfmlUtils::convertPos(tiles, tiles), nullptr)
    {
    }

    bool Grid::addItem(ViewItem *item, const sf::Vector2f &pos, bool move) noexcept
    {
        if (item == nullptr)
            return false;

        if (!_items[SfmlUtils::convertPos(pos, _tiles)])
        {
            if (move)
                item->setPosition(convertPos(pos));
            item->setSize(_tileSize);
            _items[SfmlUtils::convertPos(pos, _tiles)] = item;
            return true;
        }

        return false;
    }

    void Grid::moveItem(ViewItem *item, size_t newx, size_t newy, putils::Timer::t_duration time) noexcept
    {
        size_t x, y;
        for (size_t cur = 0; cur < _tiles.x * _tiles.y; ++cur)
        {
            x = cur % (int) _tiles.x;
            y = cur / (int) _tiles.x;

            if (_items[getIndex(x, y)] == item && (x != newx || y != newy))
            {
                if (addItem(item, sf::Vector2f((float) newx, (float) newy), false))
                {
                    item->setPreviousPosition(item->getPosition());
                    item->setDestination(convertPos(sf::Vector2f((float) newx, (float) newy)));

                    item->setDelay(time);
                    item->startMove();
                    _items[getIndex(x, y)] = nullptr;
                }
                return;
            }
        }
    }

    void Grid::displayGrid(sf::RenderWindow &window) const noexcept
    {
        for (float x = 0; x < _tiles.x; ++x)
        {
            sf::RectangleShape line(sf::Vector2f(1, _size.y));
            line.setPosition(sf::Vector2f(x * _tileSize.x, 0));
            window.draw(line);
        }

        for (float y = 0; y < _tiles.y; ++y)
        {
            sf::RectangleShape line(sf::Vector2f(_size.x, 1));
            line.setPosition(sf::Vector2f(0, y * _tileSize.y));
            window.draw(line);
        }
    }

    size_t Grid::getItemIndex(const ViewItem *item) noexcept
    {
        size_t x, y;
        for (size_t cur = 0; cur < _tiles.x * _tiles.y; ++cur)
        {
            x = cur % (int) _tiles.x;
            y = cur / (int) _tiles.x;

            if (_items[getIndex(x, y)] == item)
                return getIndex(x, y);
        }
        return (0);
    }
}
