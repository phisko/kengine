#pragma once

namespace kengine::data {
	//! putils reflect all
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

#include "animation.rpp"