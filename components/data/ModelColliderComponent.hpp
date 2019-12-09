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

			putils_reflection_class_name(ModelColliderComponentCollider);
			putils_reflection_attributes(
				putils_reflection_attribute(&Collider::shape),
				putils_reflection_attribute(&Collider::boneName),
				putils_reflection_attribute(&Collider::boundingBox),
				putils_reflection_attribute(&Collider::yaw),
				putils_reflection_attribute(&Collider::pitch),
				putils_reflection_attribute(&Collider::roll)
			);
		};

		static constexpr char vectorName[] = "ModelColliderComponentVector";
		using vector = putils::vector<Collider, KENGINE_MAX_MODEL_COLLIDERS, vectorName>;
		vector colliders;

		putils_reflection_class_name(ModelColliderComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ModelColliderComponent::colliders)
		);
	};
}