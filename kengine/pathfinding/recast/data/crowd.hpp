#pragma once

// recast
#include <DetourCrowd.h>

// kengine
#include "kengine/pathfinding/recast/helpers/unique_ptr.hpp"

namespace kengine::pathfinding::recast {
	using crowd_ptr = unique_ptr<dtCrowd, dtFreeCrowd>;

	//! putils reflect name
	//! class_name: recast_crowd
	struct crowd {
		crowd_ptr ptr = nullptr;
	};
}

#include "crowd.rpp"