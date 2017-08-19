#pragma once

#include <iostream>
#include <putils/pse/Text.hpp>
#include "Component.hpp"
#include "pse/Sprite.hpp"

class SfComponent : public kengine::Component<SfComponent>
{
public:
    SfComponent(std::string_view sprite, const sf::Vector2f& pos, const sf::Vector2f& size)
            : _viewItem(std::make_unique<pse::Sprite>(sprite, pos, size))
    {}

    SfComponent(const sf::String& str, const sf::Vector2f& pos = {0, 0},
                const sf::Color& color = sf::Color::White,
                unsigned int textSize = 18,
                std::string_view font = "resources/fonts/arial.ttf",
                const sf::Text::Style& style = sf::Text::Regular)
            : _viewItem(std::make_unique<pse::Text>(str, pos, color, textSize, font, style)),
              _fixedSize(true)
    {}

    SfComponent(std::unique_ptr<pse::ViewItem>&& viewItem)
            : _viewItem(std::move(viewItem))
    {}

public:
    pse::ViewItem& getViewItem()
    { return *_viewItem.get(); }

    const pse::ViewItem& getViewItem() const
    { return *_viewItem.get(); }

public:
    std::string toString() const final
    { return "{type:sfml}"; }

    bool isFixedSize() const { return _fixedSize; }

private:
    std::unique_ptr<pse::ViewItem> _viewItem;
    bool _fixedSize = false;
};