#pragma once

#include <putils/pse/Text.hpp>
#include "pse/Sprite.hpp"

class SfComponent {
public:
	struct Layer {
		std::string name;
		std::unique_ptr<pse::ViewItem> item;
		bool fixedSize = false;
	};

public:
	SfComponent() = default;

    SfComponent(const std::string & sprite, const sf::Vector2f& pos, const sf::Vector2f& size) {
		viewItems.push_back({ "main", std::make_unique<pse::Sprite>(sprite, pos, size) });
    }

    SfComponent(const sf::String& str, const sf::Vector2f& pos = {0, 0},
                const sf::Color& color = sf::Color::White,
                unsigned int textSize = 18,
                const std::string & font = "resources/fonts/arial.ttf",
                const sf::Text::Style& style = sf::Text::Regular) {
		viewItems.push_back({ "main", std::make_unique<pse::Text>(str, pos, color, textSize, font, style), true });
    }

    SfComponent(std::unique_ptr<pse::ViewItem>&& viewItem) {
		viewItems.push_back({ "main", std::move(viewItem) });
    }

public:
	std::vector<Layer> viewItems;
};