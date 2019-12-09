#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct MatchString : functions::BaseFunction<
			bool(const Entity &, const char *)
		> {
			putils_reflection_class_name(MatchString);
		};
	}
}