#pragma once

#include "reflection.hpp"

namespace kengine::meta {
    struct Size {
        size_t size;
    };
}

#define refltype kengine::meta::Size
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(size)
    )
};
#undef refltype