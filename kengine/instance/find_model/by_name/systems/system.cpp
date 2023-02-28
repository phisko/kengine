#include "system.hpp"

// kengine instance
#include "kengine/instance/find_model/systems/system.hpp"
#include "kengine/instance/find_model/by_name/data/instance_of_name.hpp"

namespace kengine::instance::find_model::by_name {
	using system = find_model::system<instance_of_name>;

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}