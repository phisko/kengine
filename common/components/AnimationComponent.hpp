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

		pmeta_get_class_name(AnimationComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AnimationComponent::currentAnim),
			pmeta_reflectible_attribute(&AnimationComponent::currentTime),
			pmeta_reflectible_attribute(&AnimationComponent::speed),
			pmeta_reflectible_attribute(&AnimationComponent::loop)
		);
	};

	struct AnimListComponent {
		static constexpr char stringName[] = "AnimListComponentString";
		using string = putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH, stringName>;

		struct Anim {
			string name;
			float totalTime;
			float ticksPerSecond;

			pmeta_get_class_name(AnimListComponentAnim);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Anim::name),
				pmeta_reflectible_attribute(&Anim::totalTime),
				pmeta_reflectible_attribute(&Anim::ticksPerSecond)
			);
		};

		static constexpr char vectorName[] = "AnimListComponentVector";
		putils::vector<Anim, KENGINE_MAX_ANIMATION_FILES, vectorName> allAnims;

		pmeta_get_class_name(AnimListComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AnimListComponent::allAnims)
		);
	};

	struct AnimFilesComponent {
		static constexpr char vectorName[] = "AnimFilesComponentVector";
		using vector = putils::vector<AnimListComponent::string, KENGINE_MAX_ANIMATION_FILES, vectorName>;
		vector files;

		pmeta_get_class_name(AnimFilesComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AnimFilesComponent::files)
		);
	};
}