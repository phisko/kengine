#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::main_loop {
	using execute_signature = void(float delta_time);
	struct execute : base_function<execute_signature> {};
}

#define refltype kengine::main_loop::execute
kengine_function_reflection_info;
#undef refltype