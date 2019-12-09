#pragma once

#include "EntityCreator.hpp"

namespace kengine {
	class EntityManager;

	EntityCreator * CollisionSystem(EntityManager & em);
}