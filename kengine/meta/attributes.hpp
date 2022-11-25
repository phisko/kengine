#pragma once

// putils
#include "putils/reflection_helpers/type_attributes.hpp"

namespace kengine::meta {
    struct attributes {
    	const putils::reflection::runtime::type_attributes * type_attributes;
    };
}

#define refltype kengine::meta::attributes
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(type_attributes)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::reflection::runtime::type_attributes)
	);
};
#undef refltype