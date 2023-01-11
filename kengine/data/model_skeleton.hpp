#pragma once

// stl
#include <vector>
#include <string>

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::mesh]
	struct model_skeleton {
		//! putils reflect all
		//! class_name: model_skeleton_mesh
		struct mesh {
			std::vector<std::string> bone_names;
		};
		std::vector<mesh> meshes;
	};
}

#include "model_skeleton.reflection.hpp"