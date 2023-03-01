#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::render {
	using on_mouse_captured_signature = void(entt::entity window, bool captured);
	//! putils reflect all
	//! parents: [refltype::base]
	struct on_mouse_captured : base_function<on_mouse_captured_signature> {};
}

#include "on_mouse_captured.rpp"