#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::adjustable
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(section),
		putils_reflection_attribute(values)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string),
		putils_reflection_type(refltype::value)
	);
};
#undef refltype

#define refltype kengine::data::adjustable::value
putils_reflection_info {
	putils_reflection_custom_class_name(adjustable_value);
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(bool_storage),
		putils_reflection_attribute(float_storage),
		putils_reflection_attribute(int_storage),
		putils_reflection_attribute(color_storage),
		putils_reflection_attribute(type),
		putils_reflection_attribute(enum_count)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::storage_for_bool),
		putils_reflection_type(refltype::storage_for_float),
		putils_reflection_type(refltype::storage_for_int),
		putils_reflection_type(refltype::storage_for_color),
		putils_reflection_type(putils::normalized_color)
	);
};
#undef refltype