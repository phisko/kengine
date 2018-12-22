#pragma once

#include <functional>
#include "reflection/Reflectible.hpp"
#include "not_serializable.hpp"

namespace kengine {
	class BehaviorComponent : not_serializable {
	public:
		BehaviorComponent(const std::function<void()> & func = nullptr) : func(func) {}

		std::function<void()> func = nullptr;

		pmeta_get_class_name(BehaviorComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&BehaviorComponent::func)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}
