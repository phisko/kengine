#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::text
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(font),
		putils_reflection_attribute(value),
		putils_reflection_attribute(font_size),
		putils_reflection_attribute(color),
		putils_reflection_attribute(alignment)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype

#define refltype kengine::data::text_2d
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::data::text),
		putils_reflection_type(kengine::data::on_screen)
	);
};
#undef refltype

#define refltype kengine::data::text_3d
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(kengine::data::text)
	);
};
#undef refltype