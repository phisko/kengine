#pragma once

// entt
#include <entt/entity/entity.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/point.hpp"

namespace kengine::pathfinding {
	//! putils reflect all
	//! used_types: [putils::point3f]
	struct navigation {
		entt::entity environment = entt::null; // Entity in which we are navigating. Should have a model with a nav_mesh
		putils::point3f destination{ 0.f, 0.f, 0.f };
		float search_distance = 2.f;
		float max_speed = 1.f;
	};
}

#include "navigation.rpp"