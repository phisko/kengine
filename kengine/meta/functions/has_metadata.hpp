#pragma once

// stl
#include <string_view>

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using has_metadata_signature = bool(std::string_view key);
	//! putils reflect all
	//! parents: [refltype::base]
	struct has_metadata : base_function<has_metadata_signature> {};
}

#include "has_metadata.rpp"