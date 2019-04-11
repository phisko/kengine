#pragma once

#include "reflection/Reflectible.hpp"

namespace kengine {
	class CollisionComponent : kengine::not_serializable {
	public:
		CollisionComponent(const std::function<void(kengine::GameObject &, kengine::GameObject &)> & onCollide = nullptr)
			: onCollide(onCollide) {}

		std::function<void(kengine::Entity & other)> onCollide = nullptr;

	public:
		pmeta_get_class_name(CollisionComponent);
		pmeta_get_attributes();
        pmeta_get_methods();
        pmeta_get_parents();
	};
}
