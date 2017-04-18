#pragma once

#include "ViewItem.hpp"

namespace pse
{
    template<typename sfShape>
    class Shape : public ViewItem
    {
        // Constructor
    public:
        template<typename Arg>
        Shape(Arg &&shape) noexcept
                : _shape(std::forward<Arg>(shape)) {}

        // Destructor
    public:
        virtual ~Shape() noexcept {}

        // Access to raw SFML resource
    public:
        sfShape &get() noexcept { return _shape; }

        const sfShape &get() const noexcept { return _shape; }

        // ViewItem copy
    public:
        std::unique_ptr<ViewItem> copy() const noexcept override { return std::make_unique<Shape>(_shape); }

        // ViewItem getters
    public:
        sf::Vector2f getSize() const noexcept override
        {
            auto rect = _shape.getLocalBounds();
            return sf::Vector2f(rect.width, rect.height);
        }

        void draw(sf::RenderWindow &window) noexcept override { window.draw(_shape); }

        const sf::Drawable &getDrawable() noexcept override { return _shape; }

        sf::Transformable &getTransformable() noexcept override { return _shape; }

        // Attributes
    private:
        sfShape _shape;

        // Coplien
    public:
        Shape(const Shape &) = delete;

        Shape &operator=(const Shape &) = delete;
    };
}
