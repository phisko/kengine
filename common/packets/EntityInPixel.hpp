#pragma once

#include "Entity.hpp"
#include "Point.hpp"

namespace kengine::packets {
	struct GetEntityInPixel {
		putils::Point2ui pixel;
		kengine::Entity::ID & id;
	};
}