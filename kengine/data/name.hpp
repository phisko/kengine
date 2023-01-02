#pragma once

#ifndef KENGINE_NAME_COMPONENT_MAX_LENGTH
#define KENGINE_NAME_COMPONENT_MAX_LENGTH 64
#endif

// putils
#include "putils/string.hpp"

namespace kengine::data {
	struct name {
		static constexpr char string_name[] = "name_string";
		using string = putils::string<KENGINE_NAME_COMPONENT_MAX_LENGTH, string_name>;
		string name;
	};
}

#define refltype kengine::data::name
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(name)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype
