#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct DisplayImGui : functions::BaseFunction<
			void(const Entity &)
		> {
			putils_reflection_class_name(DisplayImGui);
		};
	}
}