#pragma once

#include "SerializableComponent.hpp"

namespace kengine {
	class BehaviorComponent : public kengine::SerializableComponent<BehaviorComponent>
	{
	public:
		BehaviorComponent(const std::function<void()> & func = nullptr) : func(func) {}

		const std::string type = pmeta_nameof(BehaviorComponent);
		std::function<void()> func = nullptr;

		pmeta_get_class_name(BehaviorComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&BehaviorComponent::type),
			pmeta_reflectible_attribute(&BehaviorComponent::func)
		);
	};
}