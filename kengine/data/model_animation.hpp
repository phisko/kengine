#pragma once

#ifndef KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE
# define KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE 64
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
	struct model_animation {
		struct anim {
			std::string name;
			float total_time;
			float ticks_per_second;
		};

		std::vector<anim> animations; // Filled by animation system with what's read in `files` (or the model file itself)

		// using extracted_motion_getter = putils::function<putils::point3f(entt::entity e, size_t anim, float time), KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE>;
		using extracted_motion_getter = std::function<putils::point3f(entt::entity e, size_t anim, float time)>;

		extracted_motion_getter get_animation_movement_until_time;
		extracted_motion_getter get_animation_rotation_until_time;
		extracted_motion_getter get_animation_scaling_until_time;
	};
}

#define refltype kengine::data::model_animation
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(animations),
		putils_reflection_attribute(get_animation_movement_until_time),
		putils_reflection_attribute(get_animation_rotation_until_time),
		putils_reflection_attribute(get_animation_scaling_until_time)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::anim)
	);
};
#undef refltype

#define refltype kengine::data::model_animation::anim
putils_reflection_info {
	putils_reflection_custom_class_name(model_animation_anim);
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(total_time),
		putils_reflection_attribute(ticks_per_second)
	);
};
#undef refltype
