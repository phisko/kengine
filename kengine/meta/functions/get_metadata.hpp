#pragma once

// stl
#include <string_view>

// kengine
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using get_metadata_signature = const void * (std::string_view key);
	//! putils reflect all
	//! parents: [refltype::base]
	struct get_metadata : base_function<get_metadata_signature> {};
}

#include "get_metadata.rpp"

