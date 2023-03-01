#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::render {
	//! putils reflect all
	struct on_screen {
		enum class coordinate_type {
			screen_percentage,
			pixels
		};

		coordinate_type coordinates = coordinate_type::pixels;
	};
}

#include "on_screen.rpp"