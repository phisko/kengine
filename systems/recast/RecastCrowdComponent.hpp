#pragma once

#include <DetourCrowd.h>
#include "UniquePtr.hpp"

namespace kengine {
	using CrowdPtr = UniquePtr<dtCrowd, dtFreeCrowd>;

	struct RecastCrowdComponent {
		CrowdPtr crowd = nullptr;
	};
}