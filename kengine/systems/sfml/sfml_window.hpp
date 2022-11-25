#pragma once

// stl
#include <memory>

// sfml
#include <SFML/Graphics/RenderWindow.hpp>

// impl
#include "imgui-sfml/imgui-SFML.h"

namespace kengine::data {
    struct sfml_window {
        std::unique_ptr<sf::RenderWindow> window;

		sfml_window(std::unique_ptr<sf::RenderWindow> && window) noexcept
			: window(std::move(window))
		{}

		~sfml_window() noexcept {
			if (window)
				ImGui::SFML::Shutdown(*window);
		}
    };
}