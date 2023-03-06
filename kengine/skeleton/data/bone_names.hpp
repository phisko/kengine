#pragma once

// stl
#include <vector>
#include <string>

// reflection
#include "putils/reflection.hpp"

namespace kengine::skeleton {
	//! putils reflect all
	//! used_types: [refltype::mesh]
	struct bone_names {
		//! putils reflect all
		//! class_name: model_skeleton_mesh
		struct mesh {
			std::vector<std::string> bone_names;
		};
		std::vector<mesh> meshes;
	};
}

#include "bone_names.rpp"