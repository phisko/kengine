#pragma once

#include <vector>
#include "vector.hpp"
#include "string.hpp"

struct TexturedModelComponent : kengine::not_serializable {
	// Used to indicate that this entity's ModelComponent is tied to a textured model
	pmeta_get_class_name(TexturedModelComponent);
};

#ifndef KENGINE_SKELETON_MAX_BONES
# define KENGINE_SKELETON_MAX_BONES 100
#endif

struct SkeletonComponent : kengine::not_serializable {
	unsigned int currentAnim = 0; // Index into SkeletonInfoComponent.allAnims
	float currentTime = 0.f;

	struct Mesh {
		putils::vector<glm::mat4, KENGINE_SKELETON_MAX_BONES> boneMats;
	};
	std::vector<Mesh> meshes;

	pmeta_get_class_name(SkeletonComponent);
};

struct SkeletonInfoComponent : kengine::not_serializable {
	struct Anim {
		putils::string<64> name;
		float totalTime;
		float ticksPerSecond;
	};
	std::vector<Anim> allAnims;

	pmeta_get_class_name(SkeletonInfoComponent);
};
