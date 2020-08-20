#pragma once

#include "Point.hpp"

#ifndef KENGINE_BONE_NAME_MAX_LENGTH
# define KENGINE_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_MAX_MODEL_COLLIDERS
# define KENGINE_MAX_MODEL_COLLIDERS 64
#endif

#include "string.hpp"
#include "vector.hpp"

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
			putils::Rect3f boundingBox = { {}, { 1.f, 1.f, 1.f } };
			float yaw = 0.f;
			float pitch = 0.f;
			float roll = 0.f;
		};

		static constexpr char vectorName[] = "ModelColliderComponentVector";
		using vector = putils::vector<Collider, KENGINE_MAX_MODEL_COLLIDERS, vectorName>;
		vector colliders;
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
		putils_reflection_attribute(boundingBox),
		putils_reflection_attribute(yaw),
		putils_reflection_attribute(pitch),
		putils_reflection_attribute(roll)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::string)
	);
};
#undef refltype