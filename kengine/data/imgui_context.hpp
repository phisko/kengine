#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct imgui_context {
		void * context;
	};
}

#define refltype kengine::data::imgui_context
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(context)
	);
};
#undef refltype