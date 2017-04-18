#include "ViewItem.hpp"

namespace pse
{
    ViewItem::ViewItem() noexcept
            :
            _pos(0, 0),
            _destination(-1, -1),
            _moveTimer(-1) {}

    void ViewItem::setPosition(const sf::Vector2f &pos) noexcept
    {
        _pos = pos;
        getTransformable().setPosition(pos);
    }

    void ViewItem::setSize(const sf::Vector2f &size) noexcept
    {
        sf::Vector2f scale(size.x / getSize().x, size.y / getSize().y);
        getTransformable().setScale(scale);
    }

    void ViewItem::setX(double x) noexcept
    {
        _pos.x = (float) x;
        getTransformable().setPosition(_pos);
    }

    void ViewItem::setY(double y) noexcept
    {
        _pos.y = (float) y;
        getTransformable().setPosition(_pos);
    }

    void ViewItem::goTo(const sf::Vector2f &destination, double time) noexcept
    {
        setPreviousPosition(getPosition());
        setDestination(destination);
        setDelay(time);
        startMove();
    }
}
