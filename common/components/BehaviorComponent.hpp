#pragma once

#ifndef KENGINE_BEHAVIOR_FUNCTION_SIZE
# define KENGINE_BEHAVIOR_FUNCTION_SIZE 64
#endif

#include "function.hpp"
#include "reflection/Reflectible.hpp"
#include "not_serializable.hpp"

namespace kengine {
	class BehaviorComponent : not_serializable {
	public:
		using function = putils::function<void(), KENGINE_BEHAVIOR_FUNCTION_SIZE>;

		BehaviorComponent() = default;

		template<typename Func>
		BehaviorComponent(Func && func) : func(FWD(func)) {}

		function func = nullptr;

		pmeta_get_class_name(BehaviorComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&BehaviorComponent::func)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}
