#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::sprite_2d
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::data::on_screen)
	);
};
#undef refltype

#define refltype kengine::data::sprite_3d
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype