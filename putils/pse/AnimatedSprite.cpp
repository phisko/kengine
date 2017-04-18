#include "AnimatedSprite.hpp"

namespace pse
{
    AnimatedSprite::AnimatedSprite(
            const std::string &texture,
            const sf::Vector2f &pos,
            const sf::Vector2f &size,
            size_t animations,
            const std::vector<size_t> &tilesPerAnimation,
            const sf::Vector2f &tileDimensions,
            double refreshDelay) noexcept
            :
            Sprite(texture, pos, size),
            _tilesPerAnimation(tilesPerAnimation),
            _originalRefreshDelay(refreshDelay),
            _animated(false),
            _animations(animations),
            _currentAnimation(0),
            _currentTile(0),
            _lastTile(tilesPerAnimation[0]),
            _tileDimensions(tileDimensions),
            _refreshTimer(refreshDelay)
    {
        if (_tileDimensions == sf::Vector2f(0, 0))
            _tileDimensions = size;
        AnimatedSprite::setTexture(texture);
    }

    std::unique_ptr<ViewItem> AnimatedSprite::copy() const noexcept
    {
        return std::make_unique<AnimatedSprite>(
                getTextureFile(),
                getPosition(),
                getSize(),
                _animations,
                _tilesPerAnimation,
                getTileDimensions(),
                _originalRefreshDelay.count()
        );
    }

    void AnimatedSprite::setTexture(const std::string &texture) noexcept
    {
        Sprite::setTexture(texture);
        updateTexture();
    }

    void AnimatedSprite::advance() noexcept
    {
        ++_currentTile;
        if (_currentTile >= _lastTile)
        {
            stopAnimation();
            return;
        }

        updateTexture();
    }

    void AnimatedSprite::refresh() noexcept
    {
        if (!_animated || _refreshTimer.getDuration().count() <= 0)
            return;

        if (_refreshTimer.isDone())
        {
            _refreshTimer.restart();
            advance();
        }
    }

    void AnimatedSprite::startAnimation(const std::chrono::duration<double> &time) noexcept
    {
        _animated = true;
        if (time != std::chrono::duration<double>(-1))
            _refreshTimer.setDuration(time / _tilesPerAnimation[_currentAnimation]);
    }

    void AnimatedSprite::stopAnimation() noexcept
    {
        _animated = false;
        _currentTile = 0;
        updateTexture();
    }
}

