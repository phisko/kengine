#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	//! putils reflect all
	struct on_screen {
		enum class coordinate_type {
			screen_percentage,
			pixels
		};

		coordinate_type coordinates = coordinate_type::pixels;
	};
}

#include "on_screen.reflection.hpp"