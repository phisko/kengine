#pragma once

#ifndef KENGINE_BEHAVIOR_FUNCTION_SIZE
# define KENGINE_BEHAVIOR_FUNCTION_SIZE 64
#endif

#include "function.hpp"
#include "reflection.hpp"

namespace kengine {
	class BehaviorComponent {
	public:
		using function = putils::function<void(), KENGINE_BEHAVIOR_FUNCTION_SIZE>;

		BehaviorComponent() = default;

		template<typename Func>
		BehaviorComponent(const Func & func) : func(func) {}

		function func = nullptr;

		putils_reflection_class_name(BehaviorComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&BehaviorComponent::func)
		);
	};
}
