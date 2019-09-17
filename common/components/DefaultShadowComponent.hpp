#pragma once

#include "not_serializable.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine {
	struct DefaultShadowComponent : kengine::not_serializable {
		pmeta_get_class_name(DefaultShadowComponent);
	};
}
