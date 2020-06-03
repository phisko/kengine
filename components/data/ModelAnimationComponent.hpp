#pragma once

#ifndef KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE
# define KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE 64
#endif

#include <string>
#include <vector>
#include "Point.hpp"

namespace kengine {
	struct ModelAnimationComponent {
		std::vector<std::string> files; // Filled by user

		struct Anim {
			std::string name;
			float totalTime;
			float ticksPerSecond;

			putils_reflection_class_name(ModelAnimationComponentAnim);
			putils_reflection_attributes(
				putils_reflection_attribute(&Anim::name),
				putils_reflection_attribute(&Anim::totalTime),
				putils_reflection_attribute(&Anim::ticksPerSecond)
			);
		};

		std::vector<Anim> animations; // Filled by animation system with what's read in `files` (or the model file itself)

		using ExtractedMotionGetter = putils::function<putils::Point3f(const Entity & e, size_t anim, float time), KENGINE_ANIMATION_EXTRACTED_MOTION_FUNC_SIZE>;

		ExtractedMotionGetter getAnimationMovementUntilTime;
		ExtractedMotionGetter getAnimationRotationUntilTime;
		ExtractedMotionGetter getAnimationScalingUntilTime;

		putils_reflection_class_name(ModelAnimationComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ModelAnimationComponent::files),
			putils_reflection_attribute(&ModelAnimationComponent::animations),
			putils_reflection_attribute(&ModelAnimationComponent::getAnimationMovementUntilTime),
			putils_reflection_attribute(&ModelAnimationComponent::getAnimationRotationUntilTime),
			putils_reflection_attribute(&ModelAnimationComponent::getAnimationScalingUntilTime)
		);
		putils_reflection_used_types(
			putils_reflection_type(Anim)
		);
	};
}
