#pragma once

#ifndef KENGINE_BONE_NAME_MAX_LENGTH
#define KENGINE_BONE_NAME_MAX_LENGTH 64
#endif

#ifndef KENGINE_MAX_MODEL_COLLIDERS
#define KENGINE_MAX_MODEL_COLLIDERS 64
#endif

// putils
#include "putils/string.hpp"
#include "putils/vector.hpp"

// kengine data
#include "kengine/data/transform.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::collider]
	struct model_collider {
		//! putils reflect all
		//! class_name: model_collider_collider
		//! used_types: [refltype::string]
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

#include "model_collider.rpp"