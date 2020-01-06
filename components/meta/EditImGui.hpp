#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace meta {
		struct EditImGui : functions::BaseFunction<
			void(kengine::Entity &)
		> {
			putils_reflection_class_name(EditImGui);
		};
	}
}
