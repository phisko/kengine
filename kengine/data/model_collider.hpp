#pragma once


#ifndef KENGINE_BONE_NAME_MAX_LENGTH
# define KENGINE_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_MAX_MODEL_COLLIDERS
# define KENGINE_MAX_MODEL_COLLIDERS 64
#endif

// putils
#include "putils/string.hpp"
#include "putils/vector.hpp"

// kengine data
#include "kengine/data/transform.hpp"

namespace kengine::data {
	struct model_collider {
		struct collider {
			static constexpr char string_name[] = "model_collider_string";
			using string = putils::string<KENGINE_BONE_NAME_MAX_LENGTH, string_name>;

			enum shape_type {
				box,
				capsule,
				cone,
				cylinder,
				sphere
			};

			shape_type shape;
			string bone_name;
			data::transform transform;
		};

		std::vector<collider> colliders;
	};
}

#define refltype kengine::data::model_collider
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(colliders)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::collider)
	);
};
#undef refltype

#define refltype kengine::data::model_collider::collider
putils_reflection_info {
	putils_reflection_custom_class_name(model_collider_collider);
	putils_reflection_attributes(
		putils_reflection_attribute(shape),
		putils_reflection_attribute(bone_name),
		putils_reflection_attribute(transform)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype