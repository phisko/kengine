#pragma once

#ifndef KENGINE_MODEL_STRING_MAX_LENGTH
# define KENGINE_MODEL_STRING_MAX_LENGTH 128
#endif

// putils
#include "putils/string.hpp"

namespace kengine::data {
	struct model {
		static constexpr char string_name[] = "model_string";
		putils::string<KENGINE_MODEL_STRING_MAX_LENGTH, string_name> file;
	};
}

#define refltype kengine::data::model
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(file)
	)
};
#undef refltype