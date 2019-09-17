#pragma once

#include <putils/pse/Text.hpp>
#include "pse/Sprite.hpp"

class SfComponent {
public:
	std::unique_ptr<pse::ViewItem> item;
	bool fixedSize = false;

public:
	SfComponent() = default;

    SfComponent(const std::string & sprite, const sf::Vector2f& pos, const sf::Vector2f& size) {
		item = std::make_unique<pse::Sprite>(sprite, pos, size);
    }

    SfComponent(const sf::String& str, const sf::Vector2f& pos = {0, 0},
                const sf::Color& color = sf::Color::White,
                unsigned int textSize = 18,
                const std::string & font = "resources/fonts/arial.ttf",
                const sf::Text::Style& style = sf::Text::Regular) {
		item = std::make_unique<pse::Text>(str, pos, color, textSize, font, style);
		fixedSize = true;
    }

    SfComponent(std::unique_ptr<pse::ViewItem>&& viewItem) {
		item = std::move(viewItem);
    }

	pmeta_get_class_name(SfComponent);
};

struct SfComponentDebug {
	std::unique_ptr<pse::ViewItem> item;
	pmeta_get_class_name(SfComponentDebug);
};