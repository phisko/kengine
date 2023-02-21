#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::core::assert {
	using on_assert_failed_signature = bool (const char * file, int line, const std::string & expr);
	struct on_assert_failed : base_function<on_assert_failed_signature> {};
}

#define refltype kengine::core::assert::on_assert_failed
kengine_function_reflection_info;
#undef refltype