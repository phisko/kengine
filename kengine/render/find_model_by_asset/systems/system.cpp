#include "system.hpp"

// kengine
#include "kengine/model/find/systems/system.hpp"
#include "kengine/render/find_model_by_asset/data/instance_of_asset.hpp"

namespace kengine::render::find_model_by_asset {
	using system = kengine::model::find::system<instance_of_asset>;
	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}