#pragma once

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using emplace_or_replace_signature = void(entt::handle, const void * comp);
	//! putils reflect all
	//! parents: [refltype::base]
	struct emplace_or_replace : base_function<emplace_or_replace_signature> {};

	using emplace_or_replace_move_signature = void(entt::handle, void * comp);
	//! putils reflect all
	//! parents: [refltype::base]
	struct emplace_or_replace_move : base_function<emplace_or_replace_move_signature> {};
}

#include "emplace_or_replace.rpp"