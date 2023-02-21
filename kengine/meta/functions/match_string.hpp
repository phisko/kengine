#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using match_string_signature = bool(entt::const_handle, const char *);
	struct match_string : base_function<match_string_signature> {};
}

#define refltype kengine::meta::match_string
kengine_function_reflection_info;
#undef refltype