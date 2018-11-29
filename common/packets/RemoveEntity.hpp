#pragma once

#include "Entity.hpp"

namespace kengine::packets {
	struct RemoveEntity {
		EntityView e;
	};
}
