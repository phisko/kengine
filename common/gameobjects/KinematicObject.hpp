#pragma once

#include "Entity.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/components/PhysicsComponent.hpp"

namespace kengine {
	static inline void KinematicObject(Entity & e) {
		e.attach<TransformComponent3f>();
		e.attach<PhysicsComponent>();
	}
}
