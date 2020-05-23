#pragma once

#include "EntityManager.hpp"
#include "vector.hpp"

namespace kengine::typeHelper {
	template <typename T>
	Entity getTypeEntity(EntityManager & em);
}

// Impl
namespace kengine::typeHelper {
    template <typename T>
    Entity getTypeEntity(EntityManager & em) {
		static Entity::ID ret = Component<T>::typeEntityID([&] { return em.createEntity([](Entity &){}).id; });
        return em.getEntity(ret);
    }
}