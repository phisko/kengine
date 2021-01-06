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

		putils::Rect3f boundingBox = { {}, { 1, 1, 1 } };
		float yaw = 0.f;
		float pitch = 0.f;
		float roll = 0.f;
		bool mirrored = false;
	};
}

#define refltype kengine::ModelComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(file),

		putils_reflection_attribute(boundingBox),

		putils_reflection_attribute(yaw),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(roll),

		putils_reflection_attribute(mirrored)
	);
};
#undef refltype