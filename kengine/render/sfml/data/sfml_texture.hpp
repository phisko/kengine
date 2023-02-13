#pragma once

// sfml
#include <SFML/Graphics/Texture.hpp>

namespace kengine::data {
	//! putils reflect all
	struct sfml_texture {
		sf::Texture texture;
	};
}

#include "sfml_texture.rpp"