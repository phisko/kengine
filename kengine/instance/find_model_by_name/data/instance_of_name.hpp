#pragma once

// kengine core
#include "kengine/core/data/name.hpp"

namespace kengine::instance::find_model_by_name {
	//! putils reflect all
	//! used_types: [kengine::core::name::string]
	struct instance_of_name {
		core::name::string name;
	};
}

#include "instance_of_name.rpp"