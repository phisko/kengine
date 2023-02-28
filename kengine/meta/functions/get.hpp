#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using get_signature = void *(entt::handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct get : base_function<get_signature> {};

	using get_const_signature = const void *(entt::const_handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct get_const : base_function<get_const_signature> {};
}

#include "get.rpp"