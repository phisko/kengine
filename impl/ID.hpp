#pragma once

#include <cstddef>

namespace kengine {
	using ID = size_t;
	static constexpr ID INVALID_ID = (ID)-1;

	using EntityID = ID;
	using ComponentID = ID;
}