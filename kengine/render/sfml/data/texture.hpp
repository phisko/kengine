#pragma once

// sfml
#include <SFML/Graphics/Texture.hpp>

namespace kengine::render::sfml {
	//! putils reflect all
	//! class_name: sfml_texture
	struct texture {
		sf::Texture value;
	};
}

#include "texture.rpp"