#pragma once

// recast
#include <DetourCrowd.h>

// kengine pathfinding/recast
#include "kengine/pathfinding/recast/helpers/unique_ptr.hpp"

namespace kengine::data {
	using crowd_ptr = unique_ptr<dtCrowd, dtFreeCrowd>;

	//! putils reflect name
	struct recast_crowd {
		crowd_ptr crowd = nullptr;
	};
}

#include "recast_crowd.rpp"