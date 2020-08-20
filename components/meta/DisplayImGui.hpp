#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct DisplayImGui : functions::BaseFunction<
			void(const Entity &)
		> {};
	}
}

#define refltype kengine::meta::DisplayImGui
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype