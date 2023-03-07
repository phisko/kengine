#include "system.hpp"

// kengine
#include "kengine/model/find/systems/system.hpp"
#include "kengine/model/find/by_name/data/instance_of_name.hpp"

namespace kengine::model::find::by_name {
	using system = find::system<instance_of_name>;

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}