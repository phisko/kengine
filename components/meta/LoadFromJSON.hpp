#pragma once

#include "BaseFunction.hpp"
#include "json.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct LoadFromJSON : functions::BaseFunction<
			void(const putils::json &, kengine::Entity &)
		> {
			putils_reflection_class_name(LoadFromJSON);
		};
	}
}