#pragma once

#include <glm/glm.hpp>
#include "Point.hpp"

namespace kengine::recast {
	struct Adjustables {
		float pathOptimizationRange = 2.f;
		bool editorMode = false;
	};

	extern Adjustables g_adjustables;
}