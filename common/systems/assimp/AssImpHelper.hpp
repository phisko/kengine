#pragma once

#ifndef KENGINE_ASSIMP_BONE_INFO_PER_VERTEX
# define KENGINE_ASSIMP_BONE_INFO_PER_VERTEX 4
#endif

#ifndef KENGINE_ASSIMP_BONE_NAME_MAX_LENGTH
# define KENGINE_ASSIMP_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_ASSIMP_BONE_MAX_PARENTS
# define KENGINE_ASSIMP_BONE_MAX_PARENTS 64
#endif

#ifndef KENGINE_ASSIMP_TEXTURE_PATH_MAX_LENGTH
# define KENGINE_ASSIMP_TEXTURE_PATH_MAX_LENGTH 256
#endif

#ifndef KENGINE_ASSIMP_MODEL_PATH_MAX_LENGTH
# define KENGINE_ASSIMP_MODEL_PATH_MAX_LENGTH 256
#endif

#include "GL/glew.h"
#include "GL/GL.h"

#include "components/ModelComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkeletonComponent.hpp"

namespace kengine {
	namespace AssImpHelper {
		struct Locations {
			GLint model;
			GLint bones;
			GLint hasTexture;
			size_t diffuseTextureID;
			size_t specularTextureID;
		};

		void drawModel(kengine::EntityManager & em, const kengine::ModelComponent & model, const kengine::TransformComponent3f & transform, const kengine::SkeletonComponent & skeleton, bool useTextures, const Locations & locations);
	}
}
