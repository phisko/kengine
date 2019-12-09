#pragma once

#ifndef KENGINE_ANIMATION_FILE_PATH_LENGTH
# define KENGINE_ANIMATION_FILE_PATH_LENGTH 128
#endif

#ifndef KENGINE_MAX_ANIMATION_FILES
# define KENGINE_MAX_ANIMATION_FILES 64
#endif

#include "string.hpp"
#include "vector.hpp"

namespace kengine {
	struct AnimationComponent {
		unsigned int currentAnim = 0; // Index into AnimListComponent.allAnims
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
		static constexpr char stringName[] = "AnimListComponentString";
		using string = putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH, stringName>;

		struct Anim {
			string name;
			float totalTime;
			float ticksPerSecond;

			putils_reflection_class_name(AnimListComponentAnim);
			putils_reflection_attributes(
				putils_reflection_attribute(&Anim::name),
				putils_reflection_attribute(&Anim::totalTime),
				putils_reflection_attribute(&Anim::ticksPerSecond)
			);
		};

		static constexpr char vectorName[] = "AnimListComponentVector";
		putils::vector<Anim, KENGINE_MAX_ANIMATION_FILES, vectorName> allAnims;

		putils_reflection_class_name(AnimListComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AnimListComponent::allAnims)
		);
	};

	struct AnimFilesComponent {
		static constexpr char vectorName[] = "AnimFilesComponentVector";
		using vector = putils::vector<AnimListComponent::string, KENGINE_MAX_ANIMATION_FILES, vectorName>;
		vector files;

		putils_reflection_class_name(AnimFilesComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&AnimFilesComponent::files)
		);
	};
}