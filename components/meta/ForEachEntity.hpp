#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	namespace meta {
		struct ForEachEntity : functions::BaseFunction<
			void(const EntityIteratorFunc & func)
		> {};

		struct ForEachEntityWithout : functions::BaseFunction<
			void(const EntityIteratorFunc & func)
		> {};
	}
}

#define refltype kengine::meta::ForEachEntity
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#define refltype kengine::meta::ForEachEntityWithout
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype