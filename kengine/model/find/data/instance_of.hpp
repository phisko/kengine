#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::model::find {
	template<typename ModelComponent>
	struct instance_of {
		using base = instance_of<ModelComponent>;
		using model_component = ModelComponent;

		ModelComponent model;
	};
}

template<typename ModelComponent>
#define refltype kengine::model::find::instance_of<ModelComponent>
putils_reflection_info_template {
	putils_reflection_attributes(
		putils_reflection_attribute(model)
	);
	putils_reflection_used_types(
		putils_reflection_type(ModelComponent)
	);
};
#undef refltype