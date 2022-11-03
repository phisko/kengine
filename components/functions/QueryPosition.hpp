#pragma once

// kengine functions
#include "BaseFunction.hpp"

// putils
#include "Point.hpp"

namespace kengine::functions {
    struct QueryPosition : BaseFunction<
        void (const putils::Point3f & pos, float radius, const EntityIteratorFunc & func)
    > {};
}

#define refltype kengine::functions::QueryPosition
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype