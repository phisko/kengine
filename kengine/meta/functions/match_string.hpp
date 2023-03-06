#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using match_string_signature = bool(entt::const_handle, const char *);
	//! putils reflect all
	//! parents: [refltype::base]
	struct match_string : base_function<match_string_signature> {};
}

#include "match_string.rpp"