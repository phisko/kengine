#pragma once

#ifndef KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE
#define KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE 64
#endif

// stl
#include <string>
#include <vector>

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/point.hpp"
#include "putils/function.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::anim]
	struct model_animation {
		//! putils reflect all
		//! class_name: model_animation_anim
		struct anim {
			std::string name;
			float total_time;
			float ticks_per_second;
		};

		std::vector<anim> animations; // Filled by animation system with what's read in `files` (or the model file itself)

		using extracted_motion_getter = putils::function<putils::point3f(entt::entity e, size_t anim, float time), KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE>;

		extracted_motion_getter get_animation_movement_until_time;
		extracted_motion_getter get_animation_rotation_until_time;
		extracted_motion_getter get_animation_scaling_until_time;
	};
}

#include "model_animation.reflection.hpp"