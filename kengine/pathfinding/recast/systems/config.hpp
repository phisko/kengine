#pragma once

namespace kengine::pathfinding::recast {
	//! putils reflect all
	//! class_name: pathfinding_recast_config
	//! metadata: [("config", true)]
	struct config {
		float path_optimization_range = 2.f;
	};
}

#include "config.rpp"