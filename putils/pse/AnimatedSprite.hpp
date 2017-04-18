#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Sprite.hpp"

namespace pse
{
    //
    // Animated sprite encapsulation
    //
    class AnimatedSprite : public Sprite
    {
        // Constructor
    public:
        AnimatedSprite(
                const std::string &texture,
                const sf::Vector2f &pos,
                const sf::Vector2f &size,
                size_t animations,
                const std::vector<size_t> &tilesPerAnimation,
                const sf::Vector2f &tileDimensions,
                double refreshDelay
        ) noexcept;

        virtual ~AnimatedSprite() noexcept {}

        // ViewItem functions
    public:
        std::unique_ptr<ViewItem> copy() const noexcept override;

        void draw(sf::RenderWindow &window) noexcept override
        {
            window.draw(getDrawable());
            refresh();
        }

        // Start the animation, which should take a certain time to complete
    public:
        virtual void startAnimation(const putils::Timer::t_duration &time = putils::Timer::t_duration(-1)) noexcept;

        // Stop the animation
    public:
        virtual void stopAnimation() noexcept;

        // Update me (advance if I need to)
    public:
        virtual void refresh() noexcept;

        // Advance no matter what
    public:
        virtual void advance() noexcept;

        // Set the texture to the right tile
    public:
        virtual void updateTexture() noexcept
        {
            _sprite.setTextureRect(sf::IntRect(
                    (int) (_currentTile * _tileDimensions.x),
                    (int) (_currentAnimation * _tileDimensions.y),
                    (int) (_tileDimensions.x),
                    (int) (_tileDimensions.y)));
        }

        // Animated getters and setters
    public:
        bool isAnimated() const noexcept { return _animated; }

        void setAnimated(bool animated) noexcept { _animated = animated; }

        // Current animation getters and setters
    public:
        size_t getCurrentAnimation() const noexcept { return _currentAnimation; }

        virtual void setAnimation(size_t animation) noexcept
        {
            _currentAnimation = animation;
            _lastTile = _tilesPerAnimation[animation];
        }

        // Number of animations getter
    public:
        size_t getAnimations() const noexcept { return _animations; }

        // Current tile getters and setters
    public:
        size_t getCurrentTile() const noexcept { return _currentTile; }

        virtual void setTile(size_t currentTile) noexcept { _currentTile = currentTile; }

        // Texture setter
    public:
        void setTexture(const std::string &texture) noexcept override;

        // Tiles getters and setters
    public:
        const std::vector<size_t> &getTilesPeranimation() const noexcept { return _tilesPerAnimation; }

        size_t getTilesInAnimation() const noexcept { return _tilesPerAnimation[_currentAnimation]; }

        sf::Vector2f getTileDimensions() const noexcept { return _tileDimensions; }

        // Refresh delay getters and setters
    public:
        double getRefreshDelay() const noexcept { return _refreshTimer.getDuration().count(); }

        void setRefreshDelay(double delay) noexcept { _refreshTimer.setDuration(delay); }

        double getOriginalRefreshDelay() const noexcept { return _originalRefreshDelay.count(); }

        // Attributes
    private:
        std::vector<size_t> _tilesPerAnimation;
        putils::Timer::t_duration _originalRefreshDelay;

    private:
        bool _animated;
        size_t _animations;
        size_t _currentAnimation;
        size_t _currentTile;
        size_t _lastTile;
        sf::Vector2f _tileDimensions;
        putils::Timer _refreshTimer;

        // Coplien
    public:
        AnimatedSprite(const AnimatedSprite &) = delete;

        AnimatedSprite &operator=(const AnimatedSprite &) = delete;

        // Testing
    public:
        static void test();

    private:
        static bool testStartRefreshDelay();

        static bool testRefreshDelay();
    };
}
