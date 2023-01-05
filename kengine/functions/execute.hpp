#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using execute_signature = void(float delta_time);
	struct execute : base_function<execute_signature> {};
}

#define refltype kengine::functions::execute
kengine_function_reflection_info;
#undef refltype