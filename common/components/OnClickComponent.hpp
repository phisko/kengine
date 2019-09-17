#pragma once

#ifndef KENGINE_ONCLICK_FUNCTION_SIZE
# define KENGINE_ONCLICK_FUNCTION_SIZE 64
#endif

#include "not_serializable.hpp"
#include "reflection/Reflectible.hpp"
#include "function.hpp"

namespace kengine {
	struct OnClickComponent : not_serializable {
		putils::function<void(), KENGINE_ONCLICK_FUNCTION_SIZE> onClick = nullptr;

		pmeta_get_class_name(OnClickComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&OnClickComponent::onClick)
		);
	};
}
