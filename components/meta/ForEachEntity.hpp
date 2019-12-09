#pragma once

#include "BaseFunction.hpp"

#ifndef KENGINE_ENTITY_ITERATOR_FUNC_SIZE
# define KENGINE_ENTITY_ITERATOR_FUNC_SIZE 64
#endif

namespace kengine {
	class Entity;
	class EntityManager;

	namespace meta {
		using EntityIteratorFunc = putils::function<void(Entity &), KENGINE_ENTITY_ITERATOR_FUNC_SIZE>;

		struct ForEachEntity : functions::BaseFunction<
			void(EntityManager &, const EntityIteratorFunc & func)
		> {
			putils_reflection_class_name(ForEachEntity);
		};

		struct ForEachEntityWithout : functions::BaseFunction<
			void(EntityManager &, const EntityIteratorFunc & func)
		> {
			putils_reflection_class_name(ForEachEntityWithout);
		};
	}
}