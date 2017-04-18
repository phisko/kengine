#pragma once

#include "AnimatedSprite.hpp"

namespace pse
{
    //
    // Composite for animated sprite, lets users combine several sprites into one
    //
    class CompositeAnimatedSprite : public AnimatedSprite
    {
        // Constructor
    public:
        CompositeAnimatedSprite(
                const std::string &texture,
                const sf::Vector2f &pos,
                const sf::Vector2f &size,
                size_t animations,
                const std::vector<size_t> &tilesPerAnimation,
                const sf::Vector2f &tileDimensions,
                double refreshDelay
        ) noexcept;

        // Destructor
    public:
        virtual ~CompositeAnimatedSprite() noexcept {}

        // ViewItem functions
    public:
        std::unique_ptr<ViewItem> copy() const noexcept override;

        void draw(sf::RenderWindow &window) noexcept override;

        void setPosition(const sf::Vector2f &pos) noexcept override;

        void setSize(const sf::Vector2f &size) noexcept override;

        void setX(double x) noexcept override;

        void setY(double y) noexcept override;

        // AnimatedSprite functions
    public:
        void stopAnimation() noexcept override;

        void updateTexture() noexcept override;

        void refresh() noexcept override;

        void advance() noexcept override;

        void setAnimation(size_t animation) noexcept override;

        void setTile(size_t currentTile) noexcept override;

        // Add a new child to be drawn ABOVE previous children
    public:
        void addChild(std::unique_ptr<AnimatedSprite> &&child) noexcept
        {
            _children.push_back(std::move(child));
        }

        // Children getter
    public:
        const std::vector<std::unique_ptr<AnimatedSprite>> &getChildren() const noexcept
        {
            return _children;
        }

        // Attributes
    private:
        std::vector<std::unique_ptr<AnimatedSprite>> _children;

        // Coplien
    public:
        CompositeAnimatedSprite(const CompositeAnimatedSprite &) = delete;

        CompositeAnimatedSprite &operator=(const CompositeAnimatedSprite &) = delete;
    };
}