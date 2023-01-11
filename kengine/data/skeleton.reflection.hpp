#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::skeleton
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

#define refltype kengine::data::skeleton::mesh
putils_reflection_info {
	putils_reflection_custom_class_name(skeleton_mesh);
	putils_reflection_attributes(
		putils_reflection_attribute(bone_mats_bone_space),
		putils_reflection_attribute(bone_mats_mesh_space)
	);
};
#undef refltype