#pragma once

#include "putils/reflection.hpp"

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