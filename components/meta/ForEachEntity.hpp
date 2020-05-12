#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	namespace meta {
		struct ForEachEntity : functions::BaseFunction<
			void(const EntityIteratorFunc & func)
		> {
			putils_reflection_class_name(ForEachEntity);
		};

		struct ForEachEntityWithout : functions::BaseFunction<
			void(const EntityIteratorFunc & func)
		> {
			putils_reflection_class_name(ForEachEntityWithout);
		};
	}
}