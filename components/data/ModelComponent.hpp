#pragma once

#ifndef KENGINE_MODEL_STRING_MAX_LENGTH
# define KENGINE_MODEL_STRING_MAX_LENGTH 128
#endif

#include "string.hpp"
#include "Rect.hpp"

namespace kengine {
	struct ModelComponent {
		static constexpr char stringName[] = "ModelComponentString";
		putils::string<KENGINE_MODEL_STRING_MAX_LENGTH, stringName> file;
	};
}

#define refltype kengine::ModelComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(file)
	)
};
#undef refltype