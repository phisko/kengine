#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct on_screen {
		enum class coordinate_type {
			screen_percentage,
			pixels
		};

		coordinate_type coordinates = coordinate_type::pixels;
	};
}

#define refltype kengine::data::on_screen
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(coordinates)
	);
};
#undef refltype
