#pragma once

// entt
#include <entt/entity/entity.hpp>

namespace kengine::data {
	//! putils reflect all
	struct instance {
		entt::entity model = entt::null;
	};
}

#include "instance.reflection.hpp"