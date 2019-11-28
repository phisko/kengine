#pragma once

#include "BaseFunction.hpp"
#include "function.hpp"

#ifndef KENGINE_ENTITY_ITERATOR_FUNC_SIZE
# define KENGINE_ENTITY_ITERATOR_FUNC_SIZE 64
#endif

namespace kengine {
	class Entity;
	class EntityManager;

	namespace functions {
		using EntityIteratorFunc = putils::function<void(Entity &), KENGINE_ENTITY_ITERATOR_FUNC_SIZE>;
		struct ForEachEntity : BaseFunction<void(EntityManager &, const EntityIteratorFunc & func)> {};
		struct ForEachEntityWithout : BaseFunction<void(EntityManager &, const EntityIteratorFunc & func)> {};
	}
}