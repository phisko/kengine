#pragma once

#ifndef PUTILS_GLM
static_assert(false, "RecastSystem requires GLM");
#else

#include <glm/glm.hpp>
#include "Point.hpp"

namespace kengine::recast {
	struct Adjustables {
		float pathOptimizationRange = 2.f;
		bool editorMode = false;
	};

	extern Adjustables g_adjustables;
}

#endif