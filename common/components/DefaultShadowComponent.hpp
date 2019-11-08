#pragma once

#include "not_serializable.hpp"
#include "reflection.hpp"

namespace kengine {
	struct DefaultShadowComponent : kengine::not_serializable {
		pmeta_get_class_name(DefaultShadowComponent);
	};
}
