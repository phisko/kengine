#pragma once

#include "Entity.hpp"

namespace kengine::packets {
	struct CaptureMouse {
		Entity::ID window = Entity::INVALID_ID;
		bool captured;
	};
}