#pragma once

#include "Entity.hpp"
#include "Point.hpp"

namespace kengine::packets {
	struct GetEntityInPixel {
		Entity::ID window;
		putils::Point2ui pixel;
		kengine::Entity::ID & id;
	};
}