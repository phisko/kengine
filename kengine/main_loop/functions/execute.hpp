#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::main_loop {
	using execute_signature = void(float delta_time);
	//! putils reflect all
	//! parents: [refltype::base]
	struct execute : base_function<execute_signature> {};
}

#include "execute.rpp"