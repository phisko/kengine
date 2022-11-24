#pragma once

// stl
#include <memory>

// sfml
#include <SFML/Graphics/RenderWindow.hpp>

// impl
#include "imgui-sfml/imgui-SFML.h"

namespace kengine {
    struct SFMLWindowComponent {
        std::unique_ptr<sf::RenderWindow> window;

		SFMLWindowComponent(std::unique_ptr<sf::RenderWindow> && window) noexcept
			: window(std::move(window))
		{}

		~SFMLWindowComponent() noexcept {
			if (window)
				ImGui::SFML::Shutdown(*window);
		}
    };
}