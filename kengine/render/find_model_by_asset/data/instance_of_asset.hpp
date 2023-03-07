#pragma once

// kengine
#include "kengine/model/find/data/instance_of.hpp"
#include "kengine/render/data/asset.hpp"

namespace kengine::render::find_model_by_asset {
	//! putils reflect all
	//! parents: [refltype::base]
	struct instance_of_asset : kengine::model::find::instance_of<asset> {};
}

#include "instance_of_asset.rpp"