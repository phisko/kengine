#pragma once

// kengine core
#include "kengine/render/data/asset.hpp"

namespace kengine::render::find_model_by_asset {
	//! putils reflect all
	//! used_types: [kengine::render::asset::string]
	struct instance_of_asset {
		asset::string asset;
	};
}

#include "instance_of_asset.rpp"