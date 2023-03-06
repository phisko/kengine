#pragma once

// stl
#include <vector>

// glm
#include <glm/glm.hpp>

// reflection
#include "putils/reflection.hpp"

#ifndef KENGINE_SKELETON_MAX_BONES
#define KENGINE_SKELETON_MAX_BONES 100
#endif

namespace kengine::skeleton {
	//! putils reflect all
	//! used_types: [refltype::mesh]
	struct bone_matrices {
		//! putils reflect all
		//! class_name: skeleton_mesh
		struct mesh {
			::glm::mat4 bone_mats_bone_space[KENGINE_SKELETON_MAX_BONES]; // Used by shader
			::glm::mat4 bone_mats_mesh_space[KENGINE_SKELETON_MAX_BONES]; // Used to get bone matrix in world space
		};
		std::vector<mesh> meshes;
	};
}

#include "bone_matrices.rpp"