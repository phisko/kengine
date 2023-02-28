#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::json {
	using save_signature = nlohmann::json(entt::const_handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct save : base_function<save_signature> {};
}

#include "save.rpp"