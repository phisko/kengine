#pragma once

// stl
#include <memory>

// sfml
#include <SFML/Graphics/RenderWindow.hpp>

namespace kengine::render::sfml {
	//! putils reflect name
	//! class_name: sfml_window
	struct window {
		std::unique_ptr<sf::RenderWindow> ptr;

		window() noexcept = default;

		window(std::unique_ptr<sf::RenderWindow> && ptr) noexcept
			: ptr(std::move(ptr)) {}

		KENGINE_RENDER_SFML_EXPORT ~window() noexcept;
	};
}

#include "window.rpp"