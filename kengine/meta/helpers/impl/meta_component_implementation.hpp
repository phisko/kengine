#pragma once

// stl
#include <type_traits>

namespace kengine::meta {
	template<typename Meta, typename T>
	struct meta_component_implementation : std::true_type {};
}