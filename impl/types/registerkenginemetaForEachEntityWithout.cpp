#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"

namespace kengine::types{
	void registerkenginemetaForEachEntityWithout() noexcept {
		kengine::registerComponents<kengine::meta::ForEachEntityWithout>();

	}
}