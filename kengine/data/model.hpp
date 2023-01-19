#pragma once

#ifndef KENGINE_MODEL_STRING_MAX_LENGTH
#define KENGINE_MODEL_STRING_MAX_LENGTH 128
#endif

// putils
#include "putils/string.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::string]
	struct model {
		static constexpr char string_name[] = "model_string";
		using string = putils::string<KENGINE_MODEL_STRING_MAX_LENGTH, string_name>;
		string file;
	};
}

#include "model.rpp"