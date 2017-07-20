//
// Created by naliwe on 7/19/17.
//

#pragma once

#include <putils/pse/ViewItem.hpp>
#include <putils/pse/Text.hpp>
#include "Component.hpp"

class SfTextComponent : public kengine::Component<SfTextComponent>
{
public:
    SfTextComponent(const sf::String& str, const sf::Vector2f& pos = {0, 0},
                    const sf::Color& color = sf::Color::White,
                    unsigned int textSize = 18,
                    std::string_view font = "resources/fonts/arial.ttf",
                    const sf::Text::Style& style = sf::Text::Regular)
            : _viewItem(std::make_unique<pse::Text>(str, pos, color, textSize, font, style))
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

