#pragma once

#ifndef KENGINE_COLLISION_FUNCTION_SIZE
# define KENGINE_COLLISION_FUNCTION_SIZE 64
#endif

#include "function.hpp"
#include "reflection.hpp"

namespace kengine {
	class CollisionComponent {
	public:
		using function = putils::function<void(kengine::Entity &, kengine::Entity &), KENGINE_COLLISION_FUNCTION_SIZE>;

		CollisionComponent() = default;

		template<typename Func>
		CollisionComponent(Func && onCollide) : onCollide(FWD(onCollide)) {}

		function onCollide = nullptr;

	public:
		putils_reflection_class_name(CollisionComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&CollisionComponent::onCollide)
		);
	};
}
