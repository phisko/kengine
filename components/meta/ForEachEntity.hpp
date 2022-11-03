#pragma once

// kengine functions
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
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype

#define refltype kengine::meta::ForEachEntityWithout
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype