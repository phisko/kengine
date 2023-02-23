#pragma once

// entt
#include <entt/entity/entity.hpp>

namespace kengine::instance {
	//! putils reflect all
	struct instance {
		entt::entity model = entt::null;
	};
}

#include "instance.rpp"