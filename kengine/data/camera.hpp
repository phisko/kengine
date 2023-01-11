#pragma once

// putils
#include "putils/rect.hpp"

namespace kengine::data {
	//! putils reflect all
	struct camera {
		putils::rect3f frustum{ { 0.f, 0.f, 0.f }, { 1.f, 1.f, 1.f } };
		float pitch = 0; // Radians
		float yaw = 0; // Radians
		float roll = 0; // Radians
		float near_plane = 1.f;
		float far_plane = 1000.f;
	};
};

#include "camera.reflection.hpp"