#pragma once

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
			putils::string<64> name;
			float totalTime;
			float ticksPerSecond;
		};
		std::vector<Anim> allAnims;

		pmeta_get_class_name(AnimListComponent);
	};

}