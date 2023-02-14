#pragma once

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/rect.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [putils::rect3f]
	struct transform {
		putils::rect3f bounding_box{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
		float yaw = 0; // Radians
		float pitch = 0; // Radians
		float roll = 0; // Radians
	};
};

#include "transform.rpp"