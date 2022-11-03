#pragma once

// stl
#include <bitset>

#ifndef KENGINE_COMPONENT_COUNT
# define KENGINE_COMPONENT_COUNT 64
#endif

namespace kengine::impl {
	using ComponentMask = std::bitset<KENGINE_COMPONENT_COUNT>;
}