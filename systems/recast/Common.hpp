#pragma once

#include <glm/glm.hpp>
#include "EntityManager.hpp"
#include "Point.hpp"

namespace kengine::recast {
	struct Adjustables {
		float pathOptimizationRange = 2.f;
	};

	extern Adjustables g_adjustables;
	extern EntityManager * g_em;
}