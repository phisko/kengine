#pragma once

// recast
#include <DetourCrowd.h>

// impl
#include "unique_ptr.hpp"

namespace kengine::data {
	using crowd_ptr = unique_ptr<dtCrowd, dtFreeCrowd>;

	struct recast_crowd {
		crowd_ptr crowd = nullptr;
	};
}