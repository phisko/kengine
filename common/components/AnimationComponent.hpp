#pragma once

#ifndef KENGINE_ANIMATION_FILE_PATH_LENGTH
# define KENGINE_ANIMATION_FILE_PATH_LENGTH 128
#endif

#include <vector>
#define PUTILS_STRING_LENGTH KENGINE_ANIMATION_FILE_PATH_LENGTH
#include "string.hpp"
#undef PUTILS_STRING_LENGTH

namespace kengine {
	struct AnimationComponent : kengine::not_serializable {
		unsigned int currentAnim = 0; // Index into AnimListComponent.allAnims
		float currentTime = 0.f;
		float speed = 1.f;

		pmeta_get_class_name(AnimationComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AnimationComponent::currentAnim),
			pmeta_reflectible_attribute(&AnimationComponent::currentTime),
			pmeta_reflectible_attribute(&AnimationComponent::speed)
		);
	};

	struct AnimListComponent : kengine::not_serializable {
		struct Anim {
			putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH> name;
			float totalTime;
			float ticksPerSecond;

			pmeta_get_class_name(AnimListComponentAnim);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Anim::name),
				pmeta_reflectible_attribute(&Anim::totalTime),
				pmeta_reflectible_attribute(&Anim::ticksPerSecond)
			);
		};
		std::vector<Anim> allAnims;

		pmeta_get_class_name(AnimListComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AnimListComponent::allAnims)
		);
	};

	struct AnimFilesComponent {
		std::vector<putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH>> files;
		pmeta_get_class_name(AnimFilesComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&AnimFilesComponent::files)
		);
	};
}