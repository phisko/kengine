#pragma once

// kengine
#include "kengine/instance/find_model/data/instance_of.hpp"
#include "kengine/render/data/asset.hpp"

namespace kengine::render::find_model_by_asset {
	//! putils reflect all
	//! parents: [refltype::base]
	struct instance_of_asset : instance::find_model::instance_of<asset> {};
}

#include "instance_of_asset.rpp"