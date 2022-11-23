#pragma once


#ifndef KENGINE_BONE_NAME_MAX_LENGTH
# define KENGINE_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_MAX_MODEL_COLLIDERS
# define KENGINE_MAX_MODEL_COLLIDERS 64
#endif

// putils
#include "string.hpp"
#include "vector.hpp"

// kengine data
#include "data/TransformComponent.hpp"

namespace kengine {
	struct ModelColliderComponent {
		struct Collider {
			static constexpr char stringName[] = "ModelColliderComponentString";
			using string = putils::string<KENGINE_BONE_NAME_MAX_LENGTH, stringName>;

			enum Shape {
				Box,
				Capsule,
				Cone,
				Cylinder,
				Sphere
			};

			Shape shape;
			string boneName;
			TransformComponent transform;
		};

		std::vector<Collider> colliders;
	};
}

#define refltype kengine::ModelColliderComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(colliders)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::Collider)
	);
};
#undef refltype

#define refltype kengine::ModelColliderComponent::Collider
putils_reflection_info {
	putils_reflection_custom_class_name(ModelColliderComponentCollider);
	putils_reflection_attributes(
		putils_reflection_attribute(shape),
		putils_reflection_attribute(boneName),
		putils_reflection_attribute(transform)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype