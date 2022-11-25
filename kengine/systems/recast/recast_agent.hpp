#pragma once

// entt
#include <entt/entity/entity.hpp>

namespace kengine::data {
	struct recast_agent {
		int index = 0;
		entt::entity crowd = entt::null;
	};
}