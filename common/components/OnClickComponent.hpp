#pragma once

#ifndef KENGINE_ONCLICK_FUNCTION_SIZE
# define KENGINE_ONCLICK_FUNCTION_SIZE 64
#endif

#include "reflection.hpp"
#include "function.hpp"

namespace kengine {
	struct OnClickComponent {
		putils::function<void(), KENGINE_ONCLICK_FUNCTION_SIZE> onClick = nullptr;

		putils_reflection_class_name(OnClickComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&OnClickComponent::onClick)
		);
	};
}
