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

			pmeta_get_class_name(ModelColliderComponentCollider);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Collider::shape),
				pmeta_reflectible_attribute(&Collider::boneName),
				pmeta_reflectible_attribute(&Collider::boundingBox),
				pmeta_reflectible_attribute(&Collider::yaw),
				pmeta_reflectible_attribute(&Collider::pitch),
				pmeta_reflectible_attribute(&Collider::roll)
			);
		};

		static constexpr char vectorName[] = "ModelColliderComponentVector";
		using vector = putils::vector<Collider, KENGINE_MAX_MODEL_COLLIDERS, vectorName>;
		vector colliders;

		pmeta_get_class_name(ModelColliderComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ModelColliderComponent::colliders)
		);
	};
}