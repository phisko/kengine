#include "system.hpp"

// kengine instance/find_model
#include "kengine/instance/find_model/systems/system.hpp"

// kengine render/find_model_by_asset
#include "kengine/render/find_model_by_asset/data/instance_of_asset.hpp"

namespace kengine::render::find_model_by_asset {
	using system = instance::find_model::system<instance_of_asset>;
	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}