#pragma once

// stl
#include <vector>
#include <string>

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct model_skeleton {
		struct mesh {
			std::vector<std::string> bone_names;
		};
		std::vector<mesh> meshes;
	};
}

#define refltype kengine::data::model_skeleton
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(meshes)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::mesh)
	);
};
#undef refltype

#define refltype kengine::data::model_skeleton::mesh
putils_reflection_info {
	putils_reflection_custom_class_name(model_skeleton_mesh);
	putils_reflection_attributes(
		putils_reflection_attribute(bone_names)
	);
};
#undef refltype
