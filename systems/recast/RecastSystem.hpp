#pragma once

#include "EntityCreator.hpp"

namespace kengine {
	class EntityManager;
	EntityCreator * RecastSystem(EntityManager & em);
}