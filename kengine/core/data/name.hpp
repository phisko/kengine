#pragma once

#ifndef KENGINE_NAME_STRING_MAX_LENGTH
#define KENGINE_NAME_STRING_MAX_LENGTH 64
#endif

// putils
#include "putils/string.hpp"

namespace kengine::core {
	//! putils reflect all
	//! used_types: [refltype::string]
	struct name {
		static constexpr char string_name[] = "name_string";
		using string = putils::string<KENGINE_NAME_STRING_MAX_LENGTH, string_name>;
		string name;
	};
}

#include "name.rpp"