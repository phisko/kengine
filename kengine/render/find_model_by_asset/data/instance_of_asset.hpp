#pragma once

// kengine core
#include "kengine/render/data/asset.hpp"

// kengine instance/find_model
#include "kengine/instance/find_model/data/instance_of.hpp"

namespace kengine::render::find_model_by_asset {
	//! putils reflect all
	//! parents: [refltype::base]
	struct instance_of_asset : instance::find_model::instance_of<asset> {};
}

#include "instance_of_asset.rpp"