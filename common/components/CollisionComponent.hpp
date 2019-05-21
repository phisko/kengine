#pragma once

#ifndef KENGINE_COLLISION_FUNCTION_SIZE
# define KENGINE_COLLISION_FUNCTION_SIZE 64
#endif

#include "function.hpp"
#include "reflection/Reflectible.hpp"

namespace kengine {
	class CollisionComponent : kengine::not_serializable {
	public:
		using function = putils::function<void(kengine::Entity &, kengine::Entity &), KENGINE_COLLISION_FUNCTION_SIZE>;

		CollisionComponent() = default;

		template<typename Func>
		CollisionComponent(Func && onCollide) : onCollide(FWD(onCollide)) {}

		function onCollide = nullptr;

	public:
		pmeta_get_class_name(CollisionComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&CollisionComponent::onCollide)
		);
        pmeta_get_methods();
        pmeta_get_parents();
	};
}
