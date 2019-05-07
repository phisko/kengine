#pragma once

#ifndef KENGINE_ANIMATION_NAME_LENGTH
# define KENGINE_ANIMATION_NAME_LENGTH 64
#endif

#ifndef KENGINE_ANIMATION_FILE_PATH_LENGTH
# define KENGINE_ANIMATION_FILE_PATH_LENGTH 128
#endif

#include <vector>
#include "string.hpp"

namespace kengine {
	struct AnimationComponent : kengine::not_serializable {
		unsigned int currentAnim = 0; // Index into AnimListComponent.allAnims
		float currentTime = 0.f;
		float speed = 1.f;

		pmeta_get_class_name(AnimationComponent);
	};

	struct AnimListComponent : kengine::not_serializable {
		struct Anim {
			putils::string<KENGINE_ANIMATION_NAME_LENGTH> name;
			float totalTime;
			float ticksPerSecond;
		};
		std::vector<Anim> allAnims;

		pmeta_get_class_name(AnimListComponent);
	};

	struct AnimFilesComponent {
		std::vector<putils::string<KENGINE_ANIMATION_FILE_PATH_LENGTH>> files;
		pmeta_get_class_name(AnimFilesComponent);
	};
}