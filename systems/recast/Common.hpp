#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "Point.hpp"

namespace kengine::recast {
	struct Adjustables {
		float pathOptimizationRange = 2.f;
		bool editorMode = false;
	};

	extern Adjustables g_adjustables;
}