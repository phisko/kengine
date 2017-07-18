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

private:
    std::unique_ptr<pse::ViewItem> _viewItem;
};