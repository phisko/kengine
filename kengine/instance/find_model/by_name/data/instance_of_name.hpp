#pragma once

// kengine core
#include "kengine/core/data/name.hpp"

// kengine instance/find_model
#include "kengine/instance/find_model/data/instance_of.hpp"

namespace kengine::instance::find_model::by_name {
	//! putils reflect all
	//! parents: [refltype::base]
	struct instance_of_name : instance_of<core::name> {};
}

#include "instance_of_name.rpp"