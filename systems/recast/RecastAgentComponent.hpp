#pragma once

#include "Entity.hpp"

namespace kengine {
	struct RecastAgentComponent {
		int index = 0;
		Entity::ID crowd = Entity::INVALID_ID;
	};
}