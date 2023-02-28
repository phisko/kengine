#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::core::assert {
	using on_assert_failed_signature = bool (const char * file, int line, const std::string & expr);
	//! putils reflect all
	//! parents: [refltype::base]
	struct on_assert_failed : base_function<on_assert_failed_signature> {};
}

#include "on_assert_failed.rpp"