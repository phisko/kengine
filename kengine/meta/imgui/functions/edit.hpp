#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::imgui {
	using edit_signature = bool(entt::handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct edit : base_function<edit_signature> {};
}

#include "edit.rpp"