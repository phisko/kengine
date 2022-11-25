#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct animation {
		unsigned int current_anim = 0; // Index into data::model_animation.animations
		float current_time = 0.f;
		float speed = 1.f;
		bool loop = true;

		enum class mover_behavior { // Determines what the animation "mover" (transformations applied to the whole Entity) will be applied to
			update_transform_component,
			update_bones,
			none
		};

		mover_behavior position_mover_behavior = mover_behavior::update_bones;
		mover_behavior rotation_mover_behavior = mover_behavior::update_bones;
		mover_behavior scale_mover_behavior = mover_behavior::none;
	};
}

#define refltype kengine::data::animation
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(current_anim),
		putils_reflection_attribute(current_time),
		putils_reflection_attribute(speed),
		putils_reflection_attribute(loop),
		putils_reflection_attribute(position_mover_behavior),
		putils_reflection_attribute(rotation_mover_behavior),
		putils_reflection_attribute(scale_mover_behavior)
	);
};
#undef refltype