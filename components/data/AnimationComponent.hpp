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
		MoverBehavior scaleMoverBehavior = MoverBehavior::UpdateBones;

		putils_reflection_class_name(AnimationComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AnimationComponent::currentAnim),
			putils_reflection_attribute(&AnimationComponent::currentTime),
			putils_reflection_attribute(&AnimationComponent::speed),
			putils_reflection_attribute(&AnimationComponent::loop),
			putils_reflection_attribute(&AnimationComponent::positionMoverBehavior),
			putils_reflection_attribute(&AnimationComponent::rotationMoverBehavior),
			putils_reflection_attribute(&AnimationComponent::scaleMoverBehavior)
		);
	};
}