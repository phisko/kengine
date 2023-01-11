#pragma once

// putils
#include "putils/reflection_helpers/type_attributes.hpp"

namespace kengine::meta {
	//! putils reflect all
	//! used_types: [putils::reflection::runtime::type_attributes]
	struct attributes {
		const putils::reflection::runtime::type_attributes * type_attributes;
	};
}

#include "attributes.reflection.hpp"