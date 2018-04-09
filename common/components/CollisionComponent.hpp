#pragma once

#include "SerializableComponent.hpp"

namespace kengine {
	class CollisionComponent : public SerializableComponent<CollisionComponent> {
	public:
		CollisionComponent(const std::function<void(kengine::GameObject &, kengine::GameObject &)> & onCollide = nullptr)
			: onCollide(onCollide) {}

		std::function<void(kengine::GameObject &, kengine::GameObject &)> onCollide = nullptr;

	public:
		pmeta_get_class_name(CollisionComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&CollisionComponent::onCollide)
		);
	};
}