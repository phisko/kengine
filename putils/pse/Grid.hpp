#pragma once

#include <vector>
#include <memory>
#include "ViewItem.hpp"
#include "SfmlUtils.hpp"

namespace pse
{
    //
    // SFML Grid, simplifies easily drawing entities on a grid
    //
    class Grid
    {
        // Constructor
    public:
        Grid(const sf::Vector2f &pos, const sf::Vector2f &size, const sf::Vector2f &tiles) noexcept;

        // Add an item to grid
    public:
        bool addItem(ViewItem *item, const sf::Vector2f &pos, bool move = true) noexcept;

        // Remove an item from grid
    public:
        void removeItem(const sf::Vector2f &pos) noexcept
        {
            removeItem(SfmlUtils::convertPos(pos, _tiles));
        }

        void removeItem(size_t pos) noexcept
        {
            _items[pos] = nullptr;
        }

        // Move an item to a new position
    public:
        void
        moveItem(ViewItem *item, size_t newx, size_t newy,
                 putils::Timer::t_duration time = putils::Timer::t_duration(0)) noexcept;

        // Get the item at a specified position
    public:
        ViewItem *getItem(
                size_t x,
                size_t y) noexcept
        {
            return getItem(getIndex(x, y));
        }

        ViewItem *getItem(size_t pos) noexcept
        {
            if (pos >= _items.size())
                return nullptr;
            return _items[pos];
        }

        // Display the grid on specified window
    public:
        void displayGrid(sf::RenderWindow &window) const noexcept;

        // Get position in pixels based on position in grid
    public:
        sf::Vector2f convertPos(const sf::Vector2f &posInGrid) const noexcept
        {
            return _pos + sf::Vector2f(posInGrid.x * _tileSize.x, posInGrid.y * _tileSize.y);
        }

        // Convert 2D position to 1D position
    public:
        size_t getIndex(size_t x, size_t y) const noexcept
        {
            return SfmlUtils::convertPos(sf::Vector2f(static_cast<float>(x), static_cast<float>(y)), _tiles);
        }

        // Get the index of an item
    public:
        size_t getItemIndex(const ViewItem *item) noexcept;

        // Size getter
    public:
        const sf::Vector2f &getSize() const noexcept
        {
            return _tiles;
        }

        // Tile size getter
    public:
        const sf::Vector2f &getTileSize() const noexcept
        {
            return _tileSize;
        }

        // Attributes
    private:
        sf::Vector2f _pos;
        sf::Vector2f _size;
        sf::Vector2f _tiles;
        sf::Vector2f _tileSize;
        std::vector<ViewItem *> _items;

        // Coplien
    public:
        Grid(const Grid &) = delete;

        Grid &operator=(const Grid &) = delete;
    };
}
