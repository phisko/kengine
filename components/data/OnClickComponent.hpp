#pragma once

#ifndef KENGINE_ONCLICK_FUNCTION_SIZE
# define KENGINE_ONCLICK_FUNCTION_SIZE 64
#endif

#include "reflection.hpp"
#include "function.hpp"

namespace kengine {
	struct OnClickComponent {
		putils::function<void(), KENGINE_ONCLICK_FUNCTION_SIZE> onClick = nullptr;
	};
}

#define refltype kengine::OnClickComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(onClick)
	);
};
#undef refltype
