#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::json {
	using load_signature = void(const nlohmann::json &, entt::handle);
	//! putils reflect all
	//! parents: [refltype::base]
	struct load : base_function<load_signature> {};
}

#include "load.rpp"