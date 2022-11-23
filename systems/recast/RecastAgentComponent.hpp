#pragma once

// entt
#include <entt/entity/entity.hpp>

namespace kengine {
	struct RecastAgentComponent {
		int index = 0;
		entt::entity crowd = entt::null;
	};
}