#pragma once

#include "reflection_helpers/Attributes.hpp"

namespace kengine::meta {
    struct Attributes {
    	const putils::reflection::runtime::Attributes * attributes;
    };
}

#define refltype kengine::meta::Attributes
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(attributes)
	);
	putils_reflection_used_types(
		putils_reflection_type(putils::reflection::runtime::Attributes)
	);
};
#undef refltype