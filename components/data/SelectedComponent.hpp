#pragma once

#include "reflection.hpp"

namespace kengine {
	struct SelectedComponent {};
}

#define refltype kengine::SelectedComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype