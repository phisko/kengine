#pragma once

// putils
#include "putils/point.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [putils::vec3f]
	struct physics {
		float mass = 1.f;

		putils::vec3f movement{ 0.f, 0.f, 0.f };
		float yaw = 0.f; // radians to turn in frame
		float pitch = 0.f; // radians to turn in frame
		float roll = 0.f; // radians to turn in frame
		bool changed = false;
	};
}

#include "physics.rpp"