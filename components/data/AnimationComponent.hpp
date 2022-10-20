#pragma once

#include "reflection.hpp"

namespace kengine {
	struct AnimationComponent {
		unsigned int currentAnim = 0; // Index into ModelAnimationComponent.animations
		float currentTime = 0.f;
		float speed = 1.f;
		bool loop = true;

		enum class MoverBehavior { // Determines what the animation "mover" (transformations applied to the whole Entity) will be applied to
			UpdateTransformComponent,
			UpdateBones,
			None
		};

		MoverBehavior positionMoverBehavior = MoverBehavior::UpdateBones;
		MoverBehavior rotationMoverBehavior = MoverBehavior::UpdateBones;
		MoverBehavior scaleMoverBehavior = MoverBehavior::None;
	};
}

#define refltype kengine::AnimationComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(currentAnim),
		putils_reflection_attribute(currentTime),
		putils_reflection_attribute(speed),
		putils_reflection_attribute(loop),
		putils_reflection_attribute(positionMoverBehavior),
		putils_reflection_attribute(rotationMoverBehavior),
		putils_reflection_attribute(scaleMoverBehavior)
	);
};
#undef refltype