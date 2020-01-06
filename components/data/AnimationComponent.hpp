#pragma once

#ifndef KENGINE_ANIMATION_FILE_PATH_LENGTH
# define KENGINE_ANIMATION_FILE_PATH_LENGTH 128
#endif

#include <string>
#include <vector>
#include "reflection.hpp"

namespace kengine {
	struct AnimationComponent {
		unsigned int currentAnim = 0; // Index into AnimListComponent.anims
		float currentTime = 0.f;
		float speed = 1.f;
		bool loop = true;

		putils_reflection_class_name(AnimationComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AnimationComponent::currentAnim),
			putils_reflection_attribute(&AnimationComponent::currentTime),
			putils_reflection_attribute(&AnimationComponent::speed),
			putils_reflection_attribute(&AnimationComponent::loop)
		);
	};

	struct AnimListComponent {
		struct Anim {
			std::string name;
			float totalTime;
			float ticksPerSecond;

			putils_reflection_class_name(AnimListComponentAnim);
			putils_reflection_attributes(
				putils_reflection_attribute(&Anim::name),
				putils_reflection_attribute(&Anim::totalTime),
				putils_reflection_attribute(&Anim::ticksPerSecond)
			);
		};

		std::vector<Anim> anims;

		putils_reflection_class_name(AnimListComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AnimListComponent::anims)
		);
	};

	struct AnimFilesComponent {
		std::vector<std::string> files;

		putils_reflection_class_name(AnimFilesComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AnimFilesComponent::files)
		);
	};
}