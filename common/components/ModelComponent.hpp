#pragma once

#ifndef KENGINE_MODEL_STRING_MAX_LENGTH
# define KENGINE_MODEL_STRING_MAX_LENGTH 64
#endif

#include "string.hpp"
#include "Point.hpp"

namespace kengine {
	struct ModelComponent {
		static constexpr char stringName[] = "ModelComponentString";
		putils::string<KENGINE_MODEL_STRING_MAX_LENGTH, stringName> file;

		putils::Rect3f boundingBox = { {}, { 1, 1, 1 } };
		float pitch = 0.f;
		float yaw = 0.f;
		float roll = 0.f;
		bool mirrored = false;

		pmeta_get_class_name(ModelComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ModelComponent::file),

			pmeta_reflectible_attribute(&ModelComponent::boundingBox),

			pmeta_reflectible_attribute(&ModelComponent::pitch),
			pmeta_reflectible_attribute(&ModelComponent::yaw),
			pmeta_reflectible_attribute(&ModelComponent::roll),

			pmeta_reflectible_attribute(&ModelComponent::mirrored)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}