#pragma once

// reflection
#include "putils/reflection.hpp"

// kengine data
#include "kengine/data/on_screen.hpp"

namespace kengine::data {
	struct sprite_2d : data::on_screen { // UI element
	};

	struct sprite_3d { // Ground feedback and such
	};
}

#define refltype kengine::data::sprite_2d
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::data::on_screen)
	);
};
#undef refltype

#define refltype kengine::data::sprite_3d
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype
