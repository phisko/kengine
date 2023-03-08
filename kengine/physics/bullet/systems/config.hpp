#pragma once

namespace kengine::physics::bullet {
	//! putils reflect all
	//! class_name: physics_bullet_config
	//! metadata: [("config", true)]
	struct config {
		bool enable_debug = false;
		bool editor_mode = false;
		float gravity = 1.f;
	};
}

#include "config.rpp"