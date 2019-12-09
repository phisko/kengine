#pragma once

#include "EntityCreator.hpp"

namespace kengine {
	class EntityManager;
	EntityCreator * BulletSystem(EntityManager & em);
}