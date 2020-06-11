#pragma once

#ifndef KENGINE_MODEL_STRING_MAX_LENGTH
# define KENGINE_MODEL_STRING_MAX_LENGTH 128
#endif

#include "string.hpp"
#include "Point.hpp"

namespace kengine {
	struct ModelComponent {
		static constexpr char stringName[] = "ModelComponentString";
		putils::string<KENGINE_MODEL_STRING_MAX_LENGTH, stringName> file;

		putils::Rect3f boundingBox = { {}, { 1, 1, 1 } };
		float yaw = 0.f;
		float pitch = 0.f;
		float roll = 0.f;
		bool mirrored = false;

		putils_reflection_class_name(ModelComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ModelComponent::file),

			putils_reflection_attribute(&ModelComponent::boundingBox),

			putils_reflection_attribute(&ModelComponent::yaw),
			putils_reflection_attribute(&ModelComponent::pitch),
			putils_reflection_attribute(&ModelComponent::roll),

			putils_reflection_attribute(&ModelComponent::mirrored)
		);
	};
}