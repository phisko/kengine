#pragma once

// recast
#include <DetourCrowd.h>

// impl
#include "UniquePtr.hpp"

namespace kengine {
	using CrowdPtr = UniquePtr<dtCrowd, dtFreeCrowd>;

	struct RecastCrowdComponent {
		CrowdPtr crowd = nullptr;
	};
}