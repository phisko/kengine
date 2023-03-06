#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using remove_signature = void(entt::handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct remove : base_function<remove_signature> {};
}

#include "remove.rpp"