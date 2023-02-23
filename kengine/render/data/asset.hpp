#pragma once

#ifndef KENGINE_RENDER_ASSET_STRING_MAX_LENGTH
#define KENGINE_RENDER_ASSET_STRING_MAX_LENGTH 128
#endif

// putils
#include "putils/string.hpp"

namespace kengine::render {
	//! putils reflect all
	//! used_types: [refltype::string]
	struct asset {
		static constexpr char string_name[] = "asset_string";
		using string = putils::string<KENGINE_RENDER_ASSET_STRING_MAX_LENGTH, string_name>;
		string file;
	};
}

#include "asset.rpp"