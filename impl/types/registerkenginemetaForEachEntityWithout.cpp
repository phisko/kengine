#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaForEachEntityWithout() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::ForEachEntityWithout'");
		kengine::registerComponents<kengine::meta::ForEachEntityWithout>();

	}
}