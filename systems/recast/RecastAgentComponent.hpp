#pragma once

// kengine
#include "Entity.hpp"

namespace kengine {
	struct RecastAgentComponent {
		int index = 0;
		EntityID crowd = INVALID_ID;
	};
}