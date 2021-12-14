#pragma once

#ifndef KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE
# define KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE 64
#endif

#include <string>
#include <vector>
#include "Point.hpp"
#include "function.hpp"

namespace kengine {
	class Entity;

	struct ModelAnimationComponent {
		struct Anim {
			std::string name;
			float totalTime;
			float ticksPerSecond;
		};

		std::vector<Anim> animations; // Filled by animation system with what's read in `files` (or the model file itself)

		using ExtractedMotionGetter = putils::function<putils::Point3f(const Entity & e, size_t anim, float time), KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE>;

		ExtractedMotionGetter getAnimationMovementUntilTime;
		ExtractedMotionGetter getAnimationRotationUntilTime;
		ExtractedMotionGetter getAnimationScalingUntilTime;
	};
}

#define refltype kengine::ModelAnimationComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(animations),
		putils_reflection_attribute(getAnimationMovementUntilTime),
		putils_reflection_attribute(getAnimationRotationUntilTime),
		putils_reflection_attribute(getAnimationScalingUntilTime)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::Anim)
	);
};
#undef refltype

#define refltype kengine::ModelAnimationComponent::Anim
putils_reflection_info {
	putils_reflection_custom_class_name(ModelAnimationComponentAnim);
	putils_reflection_attributes(
		putils_reflection_attribute(name),
		putils_reflection_attribute(totalTime),
		putils_reflection_attribute(ticksPerSecond)
	);
};
#undef refltype
